/*-------------------------------------------------------------------------
 *
 * pathoram.c
 *		  Implementation of non-recursive path-oram algorithm.
 *
 *		Original paper URL: https://eprint.iacr.org/2013/280.pdf
 *
 * This implementation shifts the responsabilit of dealing with errors that
 * may happen when writing to storage to the application using the library.
 * The only errors this libraries takes into account are out of memory errors.
 * In this case, the library simply aborts the execution.
 * This error management follows the database model, since persistent changes
 * are only written to a WAL if no error occurs. When a error does occur,
 * the database aborts and recovers a consistent state from the WAL log.
 * Furthermore, there is little the library can do when an error does occur.
 *
 * When this code runs inside an enclave, it is expected that the enclave will
 * abort if any error does occur.

 *
 * Copyright (c) 2018-2019, HASLab
 *
 * IDENTIFICATION
 *		  backend/oram/pathoram.c
 *
 *-------------------------------------------------------------------------
 */

#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "oram/oram.h"
#include "oram/logger.h"
#include "oram/orandom.h"
#include "oram/pmapdefs/pdeforam.h"


struct ORAMState
{
	unsigned int blockSize;
	/* Size of a single block in Bytes(B) */
	unsigned int treeHeight;
	/* Tree Height of the oblivious file(L) */

    //unsigned int topBucketCapacity;
    //unsigned int lowerBucketCapacity;
	unsigned int bucketCapacity;
	/* Number of buckets in a Tree node(Z) */

	char	   *file;
	/* File name of the protected file. */

	Amgr	   *amgr;

	/* Set of external functions to handle ORAM states. */
	Stash		stash;
	PMap		pmap;
    FileHandler fhandler;
    
    #ifdef STASH_COUNT
    unsigned int max;
    unsigned int nblocksStashs;
    unsigned int *blocksPerBucket;
    #endif
};

typedef unsigned int TreeNode;

typedef TreeNode *TreePath;


/* non-export function prototypes */

static unsigned int calculateTreeHeight(unsigned int minimumNumberOfNodes);

static ORAMState buildORAMState(const char *filename, unsigned int blockSize,
                                unsigned int treeHeight,
                                unsigned int bucketCapacity, Amgr *amgr);

static TreePath getTreePath(ORAMState state, unsigned int leaf);

static void initBlockList(ORAMState state, PLBList *list);

static PLBList getTreeNodes(ORAMState state, TreePath path, PLBlock block, int blkno, void *appData);

static void addBlocksToStash(ORAMState state, PLBList list, void *appData);

static void getBlocksToWrite(PLBList *blocksToWrite, unsigned int a_leaf, ORAMState state, void *appData);

void getBlocksToWrite2(PLBList treeBlocks, unsigned int a_leaf, ORAMState state, void *appData);

static void writeBlocksToStorage(PLBList list, unsigned int leaf, ORAMState state, void *appData);

/*  static int check(unsigned int  a_leaf, unsigned int  s_leaf, unsigned int  level); */

static void updateBlockLeaf(BlockNumber blkno, ORAMState state);

static void updateStashWithNewBlock(void *data, unsigned int blockSize, BlockNumber blkno, ORAMState state, void *appData);


ORAMState
init_oram(const char *file, unsigned int nblocks, unsigned int blockSize, unsigned int bucketCapacity, Amgr *amgr, void *appData)
{

	unsigned int treeHeight;
	unsigned int totalNodes;

	int			result;
	ORAMState	state = NULL;

	treeHeight = calculateTreeHeight(nblocks)-1;
	totalNodes = ((unsigned int) pow(2, treeHeight + 1)) - 1;

	
    logger(DEBUG, "Init pathoram for %d bocks with tree height %d and bucket capacity %d and total nodes %d\n", nblocks, treeHeight, bucketCapacity, totalNodes);
    state = buildORAMState(file, blockSize, treeHeight, bucketCapacity, amgr);
	
    struct TreeConfig config;
	config.treeHeight = treeHeight;

	/* Initialize external files (oblivious file, stash, possitionMap) */
	state->stash = amgr->am_stash->stashinit(state->file,treeHeight, state->blockSize, appData);
	state->pmap = amgr->am_pmap->pminit(state->file, nblocks, &config);
    
    #ifdef  STASH_COUNT
    state->max = 0;
    state->nblocksStashs = 0;
    state->blocksPerBucket = (unsigned int*) malloc(sizeof(unsigned int)*totalNodes);
    memset(state->blocksPerBucket,0, sizeof(unsigned int)*totalNodes);
    #endif
    
    totalNodes = totalNodes*bucketCapacity;

	state->fhandler = amgr->am_ofile->ofileinit(state->file, totalNodes, blockSize, appData);

	return state;

}

ORAMState
buildORAMState(const char *filename, unsigned int blockSize, unsigned int treeHeight,
               unsigned int bucketCapacity, Amgr *amgr)
{

	ORAMState	state = NULL;
	unsigned int save_errno = 0;
	int			namelen = 0;

	/* Construct ORAM state */
	save_errno = errno;
	errno = 0;
	state = (ORAMState) malloc(sizeof(struct ORAMState));

    if (state == NULL && errno == ENOMEM)
	{
		logger(OUT_OF_MEMORY, "Out Of Memory building ORAM STATE\n");
		errno = save_errno;
		abort();
	}

	errno = save_errno;

	state->blockSize = blockSize;
	state->treeHeight = treeHeight;
	state->bucketCapacity = bucketCapacity;
	namelen = strlen(filename) + 1;
	state->file = (char *) malloc(namelen);
	memcpy(state->file, filename, namelen);
	/* state->file = filename; */
	state->amgr = amgr;

	return state;
}


/**
 * Calculates the inverse of the result of a power of 2. This value will tell
 * us the minimum tree height to store all of the necessary blocks.
 *
 * e.g. If the input file size is 100K and the block size is 4K, it is
 * necessary 25 blocks to store the whole file.
 * If each bucket holds 5 blocks, the three must have at least 5 nodes.
 * Since the number of nodes in the three have to be a power of 2, it is
 * necessary to calculate the closes number of
 * nodes a tree can have. As such, by calculating the inverse of  a power of 2
 * with the log 2, we have an approximation of tree height,
 * ceil(log2(5)) = 3 = K.
 * The closest number of nodes a tree can have is 2^(3)-1 = 7 nodes
 * (2^(k)-1), which is a tree of
 * height 2 ( L = K-1, L is the tree height in the paper), which has which
 * has 2^L leaves( 2^2 = 4 leaves).
 *
 * there is one particular case in which the minimum number of blocks is an
 * actual result of the power  of 2.
 * e.g. 16 = 2^4. Since a tree, only has k-1 nodes, it will only hold 15
 * nodes, thus in this case the function increments the tree height.
 * */
unsigned int
calculateTreeHeight(unsigned int nblocks)
{
	unsigned int height,
				nNodes;

	height = (unsigned int) ceil(log2(nblocks));
	nNodes = (unsigned int) pow(2, height);

	if (nNodes - 1 >= nblocks)
	{
		return height - 1;
	}
	else
	{
		return height;
	}

}


/**
 * This function returns an array of TreeNode structures that specify
 * tree nodes that have to be retrieved to get to the input tree leaf.
 * Consider for instance a Tree with height 2, which has 2^2 = 4 leafs and
 * 2^(2+1) - 1 = 7 nodes .
 * The leafs are numbered from 0 to 3 and the nodes from 0 to 6.
 * Tree example:
 *                   0
 *                  / \
 *                 1   2
 *                /\   /\
 *               3  4 5  6
 * leafs index:  0  1 2  3
 * If the request wants to retrieve the node on leaf 0 (node 3), it also has
 * to get the the tree nodes 0 and. Since the TreeNode structure also keeps
 * the tree height, the resulting TreePath (height, treeNode) will contain the
 * following values:
 * [(2,3), (1,1), (0,0)].
 *
 * To calculate the tree nodes that have to be accessed along the path it's
 * important to notice that the number can be obtained by shifting bits in an
 * integer. The trick is that the binary representation of the integer keeps
 * track of tree path that must be done to reach a target leaf.
 * e.g:
 * For a tree with the height 2 and the target leaf 0 we can encode the tree
 * position on the value  0 + 1 << 2 =  4 (100 binary). The target node is
 * obtained by subtracting 1 and getting the tree node 3.
 * For the next level, we can shift 4, 1 bit to the right and get 2 (010
 * binary). The tree node at the second level can also be obtained by
 * subtracting 1. For the next and last level, 2 is shifted right 1 bit and
 * applying the same algorithm. This works for any tree height and any target
 * leaf.
 *
 */
TreePath
getTreePath(ORAMState state, unsigned int leaf)
{
	unsigned int currentPos = 0;
	unsigned int currentHeight = state->treeHeight;
	TreePath	path = NULL;
	TreeNode	node = 0;
	int			save_errno = 0;

	currentPos = leaf + (1 << (state->treeHeight));
	save_errno = errno;
	errno = 0;
	path = (TreePath) malloc(sizeof(TreeNode) * (currentHeight + 1));

	if (path == NULL && errno == ENOMEM)
	{
		logger(OUT_OF_MEMORY, "Out Of Memory getTreePath");
		errno = save_errno;
		abort();
	}

	while (currentPos > 0)
	{
        //logger(DEBUG, "Current Pos is %d\n", currentPos);
		node = currentPos - 1;
		path[currentHeight] = node;
		currentHeight--;
		currentPos >>= 1;
	}
	errno = save_errno;

	return path;
}

void
initBlockList(ORAMState state, PLBList *list)
{
	int			save_errno = errno;

	errno = 0;
	unsigned int size = sizeof(PLBlock) * (state->treeHeight + 1) * state->bucketCapacity;

	*list = (PLBList) malloc(size);

	if (*list == NULL && errno == ENOMEM)
	{
		logger(OUT_OF_MEMORY, " Out of memory initBlockList");
		errno = save_errno;
		abort();
	}
	errno = save_errno;
}

PLBList
getTreeNodes(ORAMState state, TreePath path, PLBlock block, int blkno,  void *appData)
{
	int			level;
	int			offset;
	BlockNumber ob_blkno = 0;

	/* Oblivious file Block Number */
	PLBList		list = NULL;
	PLBlock		plblock = NULL;
	int			index = 0;
	int			prev = 0;

	int			lcapacity;
	BlockNumber lob_blkno;

	initBlockList(state, &list);

	for (level = 0; level < state->treeHeight + 1; level++)
	{

		lcapacity = level * state->bucketCapacity;
		lob_blkno = path[level] * state->bucketCapacity;

		for (offset = 0; offset < state->bucketCapacity; offset++)
		{
			ob_blkno = lob_blkno + offset;
			index = lcapacity + offset;

			plblock = createEmptyBlock();

			state->amgr->am_ofile->ofileread(state->fhandler,
                                             plblock, 
                                             state->file, 
                                             (BlockNumber) ob_blkno, 
                                             appData);


		    list[index] = plblock;

            if(plblock->blkno == blkno){
                logger(DEBUG, "Found target block in storage %s\n", plblock->block);
                block->block = malloc(plblock->size);
                memcpy(block->block, plblock->block, plblock->size);
                block->blkno = blkno;
                block->size = plblock->size;
                //free(plblock->block);
                //free(plblock);
                //list[index] = createEmptyBlock();
                //list[index]->block = malloc(state->blockSize);
                memset(plblock->block, 0, plblock->size);
                plblock->blkno = DUMMY_BLOCK;
                
            }

            #ifdef STASH_COUNT
                if(list[index] != DUMMY_BLOCK){
                    //logger(DEBUG, "read-currentPos is %d\n", path[level]);
                    state->blocksPerBucket[path[level]] -= 1;
                }
            #endif
			}
	}

	return list;
}

void
addBlocksToStash(ORAMState state, PLBList list, void *appData)
{

	int			index = 0;

	for (index = 0; index < (state->treeHeight + 1) * state->bucketCapacity; index++)
	{
		if (list[index]->blkno != DUMMY_BLOCK)
		{

            #ifdef STASH_COUNT
            state->nblocksStashs += 1;
            state->max = state->max < state->nblocksStashs? state->nblocksStashs: state->max;
            #endif
			state->amgr->am_stash->stashadd(state->stash, state->file, list[index], appData);
		}
		else
		{
			/*
			 * If it's a dummy block, a PLBlock had to be allocated and a
			 * block. This memory needs to be freed or is leaked as its is not
			 * added to the stash and there are no more references to it
			 */
			free(list[index]->block);
			free(list[index]);
		}
	}
}


void
findSBlock(ORAMState state, PLBlock t_block, int a_leaf_level,
           int s_leaf_node, int level_offset, void *appData){

    PLBlock         pl_block;
    unsigned int    s_leaf;
    AMStash         *amStash = state->amgr->am_stash;
    AMPMap          *amPmap = state->amgr->am_pmap;

    amStash->stashstartIt(state->stash, state->file, appData);

    while(amStash->stashnext(state->stash, state->file, &pl_block, appData)){
        //logger(DEBUG, "Processing stashed block %d\n", pl_block->blkno);

		s_leaf = amPmap->pmget(state->pmap, state->file, 
                               (BlockNumber) pl_block->blkno)->leaf;

		if (a_leaf_level == ((s_leaf + s_leaf_node) >> level_offset)){
            //logger(DEBUG, "moved block from stash to tree list\n");
            t_block->blkno = pl_block->blkno;
            t_block->size = pl_block->size;
            memcpy(t_block->block, pl_block->block, pl_block->size);
            amStash->stashremove(state->stash, state->file, pl_block, appData);
            break;
		 }
    }

}


void getBlocksToWrite2(PLBList treeBlocks, unsigned int a_leaf, ORAMState state, void *appData)
{
	unsigned int total = 0;

	/* Index to keep track of current tree level */
	unsigned int level = state->treeHeight + 1;
	unsigned int a_leaf_node = a_leaf + (1 << (state->treeHeight));
	unsigned int s_leaf_node = 1 << state->treeHeight;
	unsigned int index = 0;
	unsigned int level_offset = 0;
	int			a_leaf_level = 0;
	unsigned int loffset;
	unsigned int bucket_offset = 0;
    unsigned int z;

	PLBlock		pl_block;
    PLBlock     t_block;     

    
    for(; level > 0; level--){

        level_offset = ((state->treeHeight + 1) - level);
		a_leaf_level = a_leaf_node >> level_offset;
		bucket_offset = (level - 1) * state->bucketCapacity;

        for(z = 0; z < state->bucketCapacity; z++){
                
            index = bucket_offset + z;
            t_block = treeBlocks[index];
            if(t_block->blkno == DUMMY_BLOCK){
                logger(DEBUG, "Found Dummy block to replace at level %d and offset %d\n", level, z);
                findSBlock(state, t_block, a_leaf_level, 
                           s_leaf_node, level_offset, appData);

                if(t_block->blkno != DUMMY_BLOCK){
                    logger(DEBUG, "Updated list block to blkno %d and data  %s and size %d\n", t_block->blkno, t_block->block, t_block->size);
                }

            }
        }
    }

}

/***
 *
 * a_leaf -> leaf of accessed offset
 *
 */
void
getBlocksToWrite(PLBList *blocksToWrite, unsigned int a_leaf, ORAMState state, void *appData)
{

	unsigned int total = 0;

	/* Index to keep track of current tree level */
	unsigned int level = state->treeHeight + 1;
	unsigned int s_leaf = 0;
	unsigned int a_leaf_node = a_leaf + (1 << (state->treeHeight));
	unsigned int s_leaf_node = 1 << state->treeHeight;
	unsigned int index = 0;
	unsigned int level_offset = 0;
	int			a_leaf_level = 0;
	unsigned int loffset;
	unsigned int bucket_offset = 0;

	PLBlock		pl_block;
	PLBList		selectedBlocks;

	initBlockList(state, &selectedBlocks);


	for (; level > 0; level--)
	{

		state->amgr->am_stash->stashstartIt(state->stash, state->file, appData);
		level_offset = ((state->treeHeight + 1) - level);
		a_leaf_level = a_leaf_node >> level_offset;
		bucket_offset = (level - 1) * state->bucketCapacity;
        
		/* Get blocks that satisfy current level */
		while (state->amgr->am_stash->stashnext(state->stash, state->file, &pl_block, appData) && total < state->bucketCapacity)
		{

           // logger(DEBUG, "Processing block %d at level %d\n", pl_block->blkno, level);
			s_leaf = state->amgr->am_pmap->pmget(state->pmap, state->file, (BlockNumber) pl_block->blkno)->leaf;

			if (a_leaf_level == ((s_leaf + s_leaf_node) >> level_offset))
			{
                //logger(DEBUG, "ADD block to write\n");
				index = bucket_offset + total;
				selectedBlocks[index] = pl_block;
				total++;
			}
		}

		/*
		 * state->amgr->am_stash->stashcloseIt(state->stash, state->file,
		 * appData);
		 */

		/* remove from the stash selected blocks */
		for (loffset = 0; loffset < total; loffset++)
		{
			#ifdef STASH_COUNT            
            state->nblocksStashs -= 1;
            #endif
            index = bucket_offset + loffset;
			state->amgr->am_stash->stashremove(state->stash, state->file, selectedBlocks[index], appData);
		}

		/*
		 * add padding to a tree node if there werent sufficient blocks in the
		 * stash
		 */
		loffset = total;

		for (; loffset < state->bucketCapacity; loffset++)
		{
            index = bucket_offset + loffset;
			selectedBlocks[index] = createDummyBlock(state->blockSize);
		}

		total = 0;
	}

	*blocksToWrite = selectedBlocks;
}


void
writeBlocksToStorage(PLBList list, unsigned int leaf, ORAMState state, void *appData)
{
	unsigned int list_offset = (state->treeHeight + 1) * state->bucketCapacity - 1;
	unsigned int currentPos = 0;
	unsigned int index;
	BlockNumber ob_blkno = 0;
	BlockNumber lob_blkno = 0;
	PLBlock		block = NULL;
	unsigned int list_idx = 0;

	currentPos = leaf + (1 << state->treeHeight);

	while (currentPos > 0)
	{

		lob_blkno = (currentPos - 1) * state->bucketCapacity;

		for (index = 0; index < state->bucketCapacity; index++)
		{
			ob_blkno = lob_blkno + index;
			list_idx = list_offset - index;
			block = list[list_idx];

            #ifdef STASH_COUNT
                if(block->blkno != DUMMY_BLOCK)
                {
                    //logger(DEBUG, "write-currentPos is %d\n", currentPos);
                    state->blocksPerBucket[currentPos-1] +=1;
                }
            #endif
            
			state->amgr->am_ofile->ofilewrite(state->fhandler, 
                                              block,
                                              state->file,
                                              ob_blkno,
                                              appData);

			if (block->blkno != DUMMY_BLOCK)
			{

                logger(DEBUG, "Write to ob_blkno %d data %s and size %d\n", ob_blkno, block->block, block->size);
				free(block->block);
				free(block);
			}
		}
		list_offset -= state->bucketCapacity;
		currentPos >>= 1;

	}
}

void
updateBlockLeaf(BlockNumber blkno, ORAMState state)
{
	struct Location newLocation;

	BlockNumber r = ((BlockNumber) getRandomInt()) % ((BlockNumber) (pow(2, state->treeHeight)));

	newLocation.leaf = r;

	state->amgr->am_pmap->pmupdate(state->pmap, &newLocation, blkno, state->file);
}

void
updateStashWithNewBlock(void *data, unsigned int blkSize, BlockNumber blkno, ORAMState state, void *appData)
{
	PLBlock		plblock = createBlock((int) blkno, blkSize, data);
    int         found = 0;

	found = state->amgr->am_stash->stashupdate(state->stash, state->file, plblock, appData);
    
    #ifdef STASH_COUNT
    if(!found){
        state->nblocksStashs +=1;
        state->max = state->max < state->nblocksStashs? state->nblocksStashs: state->max;
    }   
    #endif
    free(plblock);
}


int
read_oram(char **ptr, BlockNumber blkno, ORAMState state, void *appData)
{
	Location	location;
	unsigned int leaf = 0;
	unsigned int result = 0;
	TreePath	path = NULL;
	PLBList		list = NULL;
	//PLBList		blocks_to_write = NULL;


	/* printf("Creating empty block\n"); */
	PLBlock		plblock = createEmptyBlock();
    PLBlock     listBlock = createEmptyBlock();


	/* printf("getting possition map\n"); */
	/* line 1 and 2 of original paper */
	location = state->amgr->am_pmap->pmget(state->pmap, state->file, blkno);
	leaf = location->leaf;
	/* printf("getting updateBlockLeaf\n"); */
	updateBlockLeaf(blkno, state);

	/* line 3 to 5 of original paper */
	path = getTreePath(state, leaf);
	list = getTreeNodes(state, path, listBlock, blkno, appData);

	/* printf("Add blocks to stash\n"); */
	//addBlocksToStash(state, list, appData);

	/* printf("Getting block to stash\n"); */
	/* Line 6 of original paper */
	state->amgr->am_stash->stashget(state->stash, plblock, blkno, state->file, appData);

    if (plblock->blkno != DUMMY_BLOCK){
        printf("target block was %d found in stash %s\n" ,plblock->blkno, plblock->block);
        free(listBlock);
    }else{
        printf("target block was %d found in storage %s\n", listBlock->blkno, listBlock->block);
        plblock->blkno = blkno;
        plblock->block = listBlock->block;
        plblock->size = listBlock->size;
        free(listBlock);
    }

    //printf("1- read string from stash %s\n", plblock->block);
	/* printf("get blocks to write\n"); */
	/* line 10 to 15 of original paper */
	getBlocksToWrite2(list, leaf, state, appData);
    //printf("2 - read string from stash %s\n", plblock->block);
    /* printf("Write blocks to storage\n"); */
	writeBlocksToStorage(list, leaf, state, appData);
    //printf("3- read string from stash %s\n", plblock->block);
	/* Free Resources */
	free(path);
	/* The plblocks of the list cannot be freed as they may be in the stash */
	free(list);
	//free(blocks_to_write);

	*ptr = plblock->block;

	/* No block has been inserted yet */
	if (plblock->blkno == DUMMY_BLOCK)
	{
        printf("return not found\n");
		result = DUMMY_BLOCK;
	}
	else
	{
        printf("return found\n");
		result = plblock->size;
	}

	free(plblock);
	return result;

}

int
write_oram(char *data, unsigned int blkSize, BlockNumber blkno, ORAMState state, void *appData)
{
	Location	location;
	unsigned int leaf = 0;
	TreePath	path = NULL;
	PLBList		list = NULL;
	//PLBList		blocks_to_write = NULL;


    PLBlock     listBlock = createEmptyBlock();


	/* line 1 and 2 of original paper */
	location = state->amgr->am_pmap->pmget(state->pmap, state->file, blkno);
	leaf = location->leaf;
	updateBlockLeaf(blkno, state);

	/* line 3 to 5 of original paper */
	path = getTreePath(state, leaf);
	list = getTreeNodes(state, path, listBlock, blkno, appData);
    
    free(listBlock->block);
    free(listBlock);
	//addBlocksToStash(state, list, appData);
    //TODO: add block list to getBlocksToWrite
	/* line 7 to 9 of original paper */
	updateStashWithNewBlock(data, blkSize, blkno, state, appData);

	/* line 10 to 15 of original paper */
	getBlocksToWrite2(list, leaf, state, appData);
	writeBlocksToStorage(list, leaf, state, appData);
    
    stashPrint(state->stash);
	/* Free Resources */
	free(path);
	/* The plblocks of the list cannot be freed as they may be in the stash */
	free(list);
	//free(blocks_to_write);

	return blkSize;
}

void
close_oram(ORAMState state, void *appData)
{

    #ifdef STASH_COUNT
    logStashes(state);
    #endif    
	state->amgr->am_stash->stashclose(state->stash, state->file, appData);
	state->amgr->am_pmap->pmclose(state->pmap, state->file);
	state->amgr->am_ofile->ofileclose(state->fhandler, state->file, appData);
    
    #ifdef STASH_COUNT
    free(state->blocksPerBucket);
    #endif
	free(state->file);
	free(state->amgr->am_stash);
	free(state->amgr->am_pmap);
	free(state->amgr->am_ofile);
	free(state);
}

#ifdef STASH_COUNT
void
logStashes(ORAMState state){
    int nbuckets;
    int bucket;
    int level;
    int total=0;
    int current = 0;
    int level_total = 0;
    int max = 0;
    int total_blocks = 0;

    nbuckets = ((unsigned int) pow(2, state->treeHeight + 1)) - 1;


    logger(DEBUG, "Stash has %d blocks and max is %d\n", state->nblocksStashs, state->max);
    
    //for(bucket = 0; bucket < nbuckets; bucket++){
    //    logger(DEBUG, "Bucket %d has %d real blocks\n", bucket, state->blocksPerBucket[bucket]);
    //}

    for(level = 0; level < state->treeHeight+1; level++){
            
        for(current = 0; current < pow(2,level); current++){
            level_total += state->blocksPerBucket[total];
            total_blocks += state->blocksPerBucket[total];
            if(state->blocksPerBucket[total]> max){
                max = state->blocksPerBucket[total];
            }
            total+=1;
        }
        logger(DEBUG, "Level %d has %d real blocks, max %d and average %f\n", level, level_total, max, level_total/(pow(2,level)*state->bucketCapacity));
        level_total = 0;
        max = 0;
    }
    logger(DEBUG, "Total number of real blocks is %d", total_blocks);
}
#endif    
