/*-------------------------------------------------------------------------
 *
 * foresoram.c
 *		  Implementation of non-recursive forst-oram algorithm.
 *
 *		Original paper URL: https://eprint.iacr.org/2013/280.pdf
 *
 * This implementation shifts the respectability of dealing with errors that
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


#include "oram/foram.h"
#include "oram/logger.h"
#include "oram/orandom.h"
#include "oram/pmapdefs/foram.h"


struct ORAMState {
    unsigned int blockSize; //Size of a single block in Bytes (B)
    unsigned int treeHeight; // Tree Height of the oblivious file (L)
    unsigned int bucketCapacity; // Number of buckets in a Tree partition node (Z)

    unsigned int nPartitions; // Number of Partitions in Forest ORAM
    unsigned int partitionsHeight; // Tree height of each partition ORAM
    unsigned int partitionCapacity; // total number of nodes in a tree partition.

    char *file; // File name of the protected file

    Amgr *amgr; // Set of external functions to handle ORAM states
    Stash* stashes;
    PMap pmap;
};

typedef unsigned int TreeNode;

typedef TreeNode *TreePath;


/* non-export function prototypes */

static unsigned int 
calculateTreeHeight(unsigned int  minimumNumberOfNodes);

static unsigned int 
calculatePartitionTreeHeight(unsigned int treeHeight);

static unsigned int 
calculateNumberOfPartitions(unsigned int treeHeight, 
							unsigned int partitionTreeHeight);

static ORAMState
buildORAMState(const char *filename, unsigned int nblocks, 
			   unsigned int fileSize, unsigned int blockSize, 
			   unsigned int minimumNumberOfNodes, unsigned int treeHeight,
			   unsigned int bucketCapacity, unsigned int nPartitions, 
			   unsigned int partitionsHeight, unsigned int partitionNodes,
			   Amgr *amgr);

static TreePath getTreePath(ORAMState state, Location leaf);

static void initBlockList(ORAMState state, PLBList *list);

static PLBList getTreeNodes(ORAMState state, TreePath path, Location location, void* appData);

static void addBlocksToStash(ORAMState state, PLBList list, Location location,  void*  appData);

static void getBlocksToWrite(PLBList *blocksToWrite, Location location, ORAMState state, void* appData);

static void writeBlocksToStorage(PLBList list, Location location, ORAMState state, void* appData);

static void updateBlockLocation(BlockNumber blkno, ORAMState state);

static void updateStashWithNewBlock(void *data, unsigned int  blockSize, BlockNumber blkno, int oldPartition, int newPartition,  ORAMState state, void* appDAta);


ORAMState init_oram(const char *file, unsigned int fileSize, unsigned int  blockSize, unsigned int  bucketCapacity, Amgr *amgr, void* appData) {

    unsigned int nblocks = 0;
    unsigned int minimumNumberOfNodes = 0;
    unsigned int treeHeight;
    unsigned int totalNodes;
    unsigned int partitionTreeHeight;
    unsigned int nPartitions;
    unsigned int partitionNodes;
    unsigned int partitionBlocks;
    int result;
    int index;

    ORAMState state = NULL;

    /*The division floors the result down to 0*/
    nblocks = fileSize / blockSize;

    /**
     * Calculates the number of leaf nodes necessary to store the number
     * of blocks in a file.
     *
     * If a file has 100 bytes with 10 blocks (N) of 10 bytes and the
     * bucketCapcity (Z) of the tree is 2 real blocks then the tree needs at
     * at least 5 nodes.
     */
    minimumNumberOfNodes = nblocks / bucketCapacity;

    /**
     * If not every bucket fits in the minimum number of tree nodes, then add
     * a new tree node to that will store the missing buckets plus a few dummy
     * nodes.
     */
    if (nblocks % bucketCapacity != 0) {
        minimumNumberOfNodes += 1;
    }

    treeHeight = calculateTreeHeight(minimumNumberOfNodes);

    totalNodes = ((unsigned int) pow(2, treeHeight + 1)) - 1;

    partitionTreeHeight = calculatePartitionTreeHeight(treeHeight);

    nPartitions = calculateNumberOfPartitions(treeHeight, partitionTreeHeight);

    partitionNodes = ((unsigned int) pow(2, partitionTreeHeight+1))-1;

    partitionBlocks = partitionNodes * nPartitions;

    if(totalNodes > partitionBlocks){
    	//logger(DEBUG, "The total number of nodes does not fit in the ORAM\n");
    	abort();
    }


    state = buildORAMState(file, nblocks, fileSize, blockSize, minimumNumberOfNodes, treeHeight, bucketCapacity, nPartitions, partitionTreeHeight, partitionNodes, amgr);

    /*Initialize external files (oblivious file, stash, possitionMap)*/
    state->stashes = (Stash*) malloc(sizeof(Stash) * nPartitions);

    for(index = 0; index < nPartitions; index++) {
       state->stashes[index] = amgr->am_stash->stashinit(state->file, state->blockSize, appData);    
    }
    
    struct TreeConfig config;
    config.treeHeight = partitionTreeHeight;
    config.nPartitions = nPartitions;

    state->pmap = amgr->am_pmap->pminit(state->file, nblocks, &config);
    amgr->am_ofile->ofileinit(state->file, partitionBlocks, blockSize, appData);

    return state;
}


ORAMState buildORAMState(const char *filename, unsigned int nblocks, 
						unsigned int fileSize, unsigned int blockSize,
						unsigned int minimumNumberOfNodes,
						unsigned int treeHeight, unsigned int bucketCapacity,
						unsigned int nPartitions, 
						unsigned int partitionTreeHeight, 
						unsigned int partitionNodes,
						Amgr *amgr) {

    ORAMState state = NULL;
    unsigned int  save_errno = 0;
    int namelen = 0;

    /*Construct ORAM state*/
    save_errno = errno;
    errno = 0;
    state = (ORAMState) malloc(sizeof(struct ORAMState));
    if (state == NULL && errno == ENOMEM) {
        logger(OUT_OF_MEMORY, "Out Of memory building ORAM state\n");
        errno = save_errno;
        abort();
    }
    errno = save_errno;

    state->blockSize = blockSize;
    state->treeHeight = treeHeight;
    state->bucketCapacity = bucketCapacity;

    state->nPartitions = nPartitions;
    state->partitionsHeight = partitionTreeHeight;
    state->partitionCapacity = partitionNodes;

    namelen = strlen(filename)+1;
    state->file = (char*) malloc(namelen);
    memcpy(state->file, filename, namelen);
    //state->file = filename;
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
unsigned int calculateTreeHeight(unsigned int minimumNumberOfNodes) {
    unsigned int height, nNodes;

    height = (unsigned int) ceil(log2(minimumNumberOfNodes));
    nNodes = (unsigned int) pow(2, height);

    if (nNodes - 1 >= minimumNumberOfNodes) {
        return height - 1;
    } else {
        return height;
    }
}


/**
	Calculates the height of the tree of each partition on Forest ORAM.
	Assuming that a classical Path ORAM required a tree with height at least treeHeight, then each partition on Forest ORAM will have only the logarithm of the required height.

	For instance, a Path ORAM with tree height 7 that can store up to 255 nodes and can be divided in 17 partitions each one storing 15 nodes, giving a total of 255 nodes.
*/
unsigned int calculatePartitionTreeHeight(unsigned int treeHeight){
	return (unsigned int) ceil(log2(treeHeight));
}


unsigned int calculateNumberOfPartitions(unsigned int treeHeight, unsigned int partitionTreeHeight){
	unsigned int nTreeNodes;
	unsigned int nSubTreeNodes;
	unsigned int nPartitions;

	nTreeNodes = (unsigned int) pow(2, treeHeight+1)-1;
	nSubTreeNodes = (unsigned int) pow(2, partitionTreeHeight+1)-1;

	nPartitions = nTreeNodes/nSubTreeNodes;

	if(nPartitions*nSubTreeNodes < nTreeNodes){
		nPartitions +=1;
	}

	return nPartitions;
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
 * to get the the tree nodes 0 and 1. Since the TreeNode structure also keeps 
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
TreePath getTreePath(ORAMState state, Location location) {
	unsigned int leaf = location->leaf;
    unsigned int currentPos = 0;
    unsigned int currentHeight = state->partitionsHeight;
    TreePath path = NULL;
    TreeNode node = 0;
    int save_errno = 0;

    currentPos = leaf + (1 << (state->partitionsHeight));
    save_errno = errno;
    errno = 0;
    path = (TreePath) malloc(sizeof(TreeNode) * (currentHeight + 1));

    if (path == NULL && errno == ENOMEM) {
        logger(OUT_OF_MEMORY, "Out Of Memory getTreePath");
        errno = save_errno;
        abort();
    }

    while (currentPos > 0) {
        node = currentPos - 1;
        path[currentHeight] = node;
        currentHeight--;
        currentPos >>= 1;
    }
    errno = save_errno;

    return path;
}

void initBlockList(ORAMState state, PLBList *list) {
    int save_errno = errno;
    errno = 0;
    unsigned int size = sizeof(PLBlock) * (state->partitionsHeight + 1) * state->bucketCapacity;
    *list = (PLBList) malloc(size);

    if (*list == NULL && errno == ENOMEM) {
        logger(OUT_OF_MEMORY, " Out of memory initBlockList");
        errno = save_errno;
        abort();
    }
    errno = save_errno;
}

PLBList getTreeNodes(ORAMState state, TreePath path, Location location, void* appData) {
    int level;
    int offset;
    BlockNumber ob_blkno = 0; // Oblivious file Block Number
    PLBList list = NULL;
    PLBlock plblock = NULL;
    int index = 0;
    int prev = 0;
    int lcapacity;
    int lob_blkno;
    unsigned int pOffset = 0; // partition offset;
    
    pOffset = location->partition*state->partitionCapacity;
    initBlockList(state, &list);

    for (level = 0; level < state->partitionsHeight + 1; level++) {
        
        lcapacity = level * state->bucketCapacity;
        lob_blkno = path[level] * state->bucketCapacity + pOffset;

        for (offset = 0; offset < state->bucketCapacity; offset++) {
            ob_blkno = lob_blkno + offset;
            index = lcapacity + offset;

            plblock = createEmptyBlock();
            state->amgr->am_ofile->ofileread(plblock, state->file, (BlockNumber) ob_blkno, appData);
            list[index] = plblock;
        }
    }

    return list;
}

void addBlocksToStash(ORAMState state, PLBList list, Location location, void* appData) {

    int index = 0;

    for (index = 0; index < (state->partitionsHeight + 1) * state->bucketCapacity; index++) {
        if (list[index]->blkno != DUMMY_BLOCK) {
            state->amgr->am_stash->stashadd(state->stashes[location->partition], 
                                            state->file, list[index], appData);
        } else {
            /*If it's a dummy block, a PLBlock had to be allocated and a block. 
            This memory needs to be freed or is leaked as its is not added to 
            the stash and there are no more references to it*/
            free(list[index]->block);
            free(list[index]);
        }
    }
}


void getBlocksToWrite(PLBList *blocksToWrite, Location a_location, ORAMState state, void* appData) {

    unsigned int  total = 0;

    // Index to keep track of current tree level
    unsigned int  level = state->partitionsHeight + 1;
    // Leaf number of accessed node
    unsigned int  a_leaf = a_location->leaf;
        
    unsigned int  a_leaf_node = a_leaf + (1 << (state->partitionsHeight));
    unsigned int  s_leaf_node = 1 << state->partitionsHeight;
    unsigned int  aux_leaf_node = 0;
    unsigned int  index;
    unsigned int  loffset;
    unsigned int level_offset = 0;
    unsigned int a_leaf_level = 0;
    unsigned int bucket_offset = 0;
  
    // Location of  a stashed node
    Location  s_location = 0;

    PLBlock pl_block;
    PLBList selectedBlocks;



    initBlockList(state, &selectedBlocks);
    
    for (; level > 0; level--) {

        state->amgr->am_stash->stashstartIt(state->stashes[a_location->partition], state->file, appData);
        
        level_offset =  (state->partitionsHeight + 1 ) - level;
        a_leaf_level =  a_leaf_node >> level_offset;
        bucket_offset = (level-1)*state->bucketCapacity;

        while (state->amgr->am_stash->stashnext(state->stashes[a_location->partition], state->file, &pl_block, appData)
               && total < state->bucketCapacity) {

            s_location = state->amgr->am_pmap->pmget(state->pmap, state->file, (BlockNumber) pl_block->blkno);

            if (a_leaf_level == ((s_location->leaf + s_leaf_node)>>level_offset)) {
                index = bucket_offset + total;
                selectedBlocks[index] = pl_block;
                total++;
            }
        }

        //state->amgr->am_stash->stashcloseIt(state->stash, state->file, appData);

        for (loffset = 0; loffset < total; loffset++) {
            index = bucket_offset + loffset;
            
            state->amgr->am_stash->stashremove(state->stashes[a_location->partition], state->file,
                                               selectedBlocks[index], appData);
        }

        
        for (loffset = total; loffset < state->bucketCapacity; loffset++) {
            index = bucket_offset + loffset;
            selectedBlocks[index] = createDummyBlock(state->blockSize);
        }

        total = 0;
    }

    *blocksToWrite = selectedBlocks;
}


void writeBlocksToStorage(PLBList list, Location location, ORAMState state, void* appData) {
    unsigned int list_offset = (state->partitionsHeight + 1) * state->bucketCapacity - 1;
    unsigned int currentPos = 0;
    unsigned int index;
    BlockNumber ob_blkno = 0;
    BlockNumber lob_blkno = 0;
    PLBlock block = NULL;
    unsigned int list_idx = 0;
    unsigned int pOffset = 0; // partition offset;

    pOffset = location->partition * state->partitionCapacity;
    currentPos = location->leaf + (1 << state->partitionsHeight);

    while (currentPos > 0) {
        
        lob_blkno = (currentPos - 1) * state->bucketCapacity + pOffset;

        for (index = 0; index < state->bucketCapacity; index++) {
            ob_blkno = lob_blkno + index;
            list_idx = list_offset - index;
            block = list[list_idx];
            state->amgr->am_ofile->ofilewrite(block, state->file, ob_blkno, appData);
            if(block->blkno != DUMMY_BLOCK){

                free(block->block);
                free(block);
            }
        }
        list_offset -= state->bucketCapacity;
        currentPos >>= 1;

    }
}

void updateBlockLocation(BlockNumber blkno, ORAMState state) {
    struct Location location;
   
    location.partition = ((BlockNumber) getRandomInt() % ((BlockNumber) state->nPartitions));
    location.leaf = ((BlockNumber) getRandomInt()) % ((BlockNumber) (pow(2, state->partitionsHeight)));

    state->amgr->am_pmap->pmupdate(state->pmap, &location, blkno, state->file);
 
}

void updateStashWithNewBlock(void *data, unsigned int  blkSize, BlockNumber blkno, int oldPartition, int newPartition, ORAMState state, void* appData) {
    
    PLBlock plblock = createBlock((int) blkno, blkSize, data);

    state->amgr->am_stash->stashtake(state->stashes[oldPartition], state->file, blkno, appData);
    state->amgr->am_stash->stashadd(state->stashes[newPartition],state->file, plblock, appData);

}


int read_foram(char **ptr, BlockNumber blkno, ORAMState state, void* appData) {
    Location location;
    unsigned int  result = 0;
    TreePath path = NULL;
    PLBList list = NULL;
    PLBlock plblock = createEmptyBlock();

    // line 1 and 2 of original paper
    location = state->amgr->am_pmap->pmget(state->pmap, state->file, blkno);

    // line 3 to 5 of original paper
    path = getTreePath(state, location);
    list = getTreeNodes(state, path, location, appData);
    
    addBlocksToStash(state, list, location,  appData);
    //Line 6 of original paper
    state->amgr->am_stash->stashget(state->stashes[location->partition], plblock, blkno,  state->file, appData);

    //Free Resources
    free(path);
    /*The plblocks of the list cannot be freed as they may be in the stash*/
    free(list);
    //free(blocks_to_write);

    *ptr = plblock->block;
    
    //No block has been inserted yet
    if (plblock->blkno == DUMMY_BLOCK) {
        result = DUMMY_BLOCK;
    }else{
        result = plblock->size;
    }

    free(plblock);
    return result;

}

int evict_foram(char *data, unsigned int  blkSize, BlockNumber blkno, ORAMState state, void* appData) {

    struct Location oldLocation;
    Location newLocation;
    PLBList blocks_to_write = NULL;
    int res; 
    
    // line 1 and 2 of original paper
    memcpy(&oldLocation, state->amgr->am_pmap->pmget(state->pmap, state->file, blkno),
            sizeof(struct Location));

    updateBlockLocation(blkno, state);

    newLocation = state->amgr->am_pmap->pmget(state->pmap, state->file, blkno);    
     

    if (blkSize != DUMMY_BLOCK){
        updateStashWithNewBlock(data, blkSize, blkno, oldLocation.partition, newLocation->partition, state, appData);
	}
    
    //line 10 to 15 of original paper
    getBlocksToWrite(&blocks_to_write, &oldLocation, state, appData);
    writeBlocksToStorage(blocks_to_write, &oldLocation, state, appData);
    free(blocks_to_write);

    return blkSize;
}

void close_oram(ORAMState state, void* appData) {
    int index = 0;

    for(index = 0; index < state->nPartitions; index++){
        state->amgr->am_stash->stashclose(state->stashes[index], state->file, appData);
    }

    state->amgr->am_pmap->pmclose(state->pmap, state->file);
    state->amgr->am_ofile->ofileclose(state->file, appData);

    free(state->stashes);
    free(state->file);
    free(state->amgr->am_stash);
    free(state->amgr->am_pmap);
    free(state->amgr->am_ofile);
    free(state);
}

int read_oram(char **ptr, BlockNumber blkno, ORAMState state, void* appData) {
	int blockSize = 0;
	blockSize = read_foram(ptr, blkno, state, appData);
	evict_foram(*ptr, (unsigned int) blockSize, blkno, state, appData);
	return blockSize;
}



int write_oram(char *data, unsigned int  blkSize, BlockNumber blkno, ORAMState state, void* appData) {
	char* tmp_data = NULL;
	int result = 0;
	result = read_foram(&tmp_data, blkno, state, appData);
	evict_foram(data, blkSize, blkno, state, appData);
	free(tmp_data);
	return blkSize;
}