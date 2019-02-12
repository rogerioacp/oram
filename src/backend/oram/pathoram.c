/*-------------------------------------------------------------------------
 *
 * pathoram.c
 *		  Implementation of non-recursive path-oram algorithm.
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
 *		  backend/pathoram.c
 *
 *-------------------------------------------------------------------------
 */

#include <errno.h>
#include <math.h>
#include <stdlib.h>

#include "oram.h"
#include "logger.h"



struct ORAMState{
	size_t nBlocks; //Number of blocks in the original file (N)
	size_t originalFileSize; //Number of bytes of the originalFileSize
	size_t blockSize; //Size of a single block in Bytes (B)
	size_t minimumNumberOfNodes; //Number of Tree nodes to store the number of blocks
	size_t treeHeight; // Tree Height of the oblivious file (L)
	size_t bucketCapacity; // Number of buckets in a Tree node (Z)

	char* file; // File name of the protected file.
	
	Amgr* amgr; // Set of external functions to handle ORAM states.
}

typedef struct TreeNode{
	size_t level;
	size_t node;
} *TreeNode;


typedef TreeNode *TreePath;

ORAMState* init(char* filename, size_t fileSize, size_t blockSize, size_t bucketCapacity, Amgr *amgr){

	size_t nBlocks = 0;
	size_t minimumNumberOfNodes = 0;
	size_t treeHeight;
	int result;
	ORAMState* state = NULL;

	/*The division floors the result down to 0*/
	nblocks = fileSize/blockSize;
	/**
	 * Calculates the number of leaf nodes necessary to store the number
	 * of blocks in a file.
	 *
	 * If a file has 100 bytes with 10 blocks (N) of 10 bytes and the
	 * bucketCapcity (Z) of the tree is 2 real blocks then the tree needs at 
	 * at least 5 nodes.
	 */
	minimumNumberOfNodes = nBlocks/bucketCapacity;

	/**
	 * If not every bucket fits in the minimum number of tree nodes, then add
	 * a new tree node to that will store the missing buckets plus a few dummy 
	 * nodes.
	 */
	if(nBlocks%bucketCapacity != 0){
		minimumNumberOfNodes +=1;
	}

	treeHeight = calculateTreeHeight(minimumNumberOfNodes);

	state = buildORAMState(file, nblocks, fileSize, blockSize, minimumNumberOfNodes, treeHeight, bucketCapacity, amgr);
	
	/*Initialize external files (oblivious file, stash, possitionMap)*/
	amgr->am_stash->stashinit(state->file);
	amgr->am_pmap->pminit(state->file, state->blockSize);
	amgr->am_ofile->ofileinit(state->file, state->nBlocks, state->treeHeight);

	return state;
	
}

ORAMState* buildORAMState(char* filename, size_t nblocks, size_t fileSize, size_t blockSize, size_t minimumNumberOfNodes, size_t treeHeight, size_t bucketCapacity, Amgr *amgr){
	size_t save_errno = 0;

	/*Construct ORAM state*/
	save_errno = errno;
	errno = 0;
	ORAMState* state = (ORAMState*) malloc(sizeof(ORAMState));
	if(r_block == NULL && errno = ENOMEM){
				elog(OUT_OF_MEMORY);
				errno = save_errno;
				abort();
	}
	errno = save_errno;

	state->nBlocks = nBlocks;
	state->fileSize = fileSize;
	state->blockSize = blockSize;
	state->minimumNumberOfNodes = minimumNumberOfNodes;
	state->treeHeight = treeHeight;
	state->bucketCapacity = bucketCapacity;

	state->file = filename;

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
size_t calculateTreeHeight(size_t minimumNumberOfNodes){
	size_t height, nNodes;

	height = (size_t) ceil(log2(minimumNumberOfNodes));
	nNodes = (size_t) pow(2, height);

	if(nNodes - 1 >=  minimumNumberOfNodes){
		return height -1;
	}else{
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
TreePath getTreePath(ORAMState *state, unsigned int leaf){
	unsigned int currentPos = 0;
	unsigned int index = 0;
	unsigned int currentHeight = state->treeHeight;
	TreePath path = NULL;
	TreeNode node = NULL;
	int save_errno = 0;

	currentPosition = leaf + ( 1 << (state->treeHeight));
	save_errno = errno;
	errno = 0;
	path = (TreePath) malloc(sizeof(TreeNode)*(currentHeight+1));

	if(path == NULL && errno = ENOMEM){
		elog(OUT_OF_MEMORY);
		errno = save_errno;
		abort();
	}



    while (currentPosition > 0) {
    	node = (TreeNode) malloc(sizeof(struct TreeNode));

    	if(node == NULL && errno = ENOMEM){
			elog(OUT_OF_MEMORY);
			errno = save_errno;
			abort();
		}

    	node->level = currentHeight;
    	node->node = currentPosition - 1;
    	path[currentHeight] = node;
        currentHeight -=1;
        currentPosition >>= 1;
    }
    errno = save_errno;

    return path;
}

void initBlockList(ORAMState *state, BlockList *list){
	size_t save_errno = errno;
	errno = 0;

	*list = (BlockList) malloc(sizeof(PLBlock)*state->treeHeight*state->bucketCapcity);

	if(list == NULL && errno == ENOMEM){
		log(OUT_OF_MEMORY);
		errno= save_errno;
		abort();
	}
	errno = save_errno;
}

PLBList getTreeNodes(ORAMState *state, TreePath path){
	size_t level = 0;
	size_t offset = 0;
	BlockNumber ob_blkno = 0; // Oblivious file Block Number
	PLBList list = NULL;
	PLBlock plblock = NULL;

	initBlockList(state, &list);


	for(level; level < state->treeHeight; level++){
		for(offset; offset < state->bucketCapacity; offset++){
			ob_blkno = (path[level].node)*state->bucketCapacity+offset;
			state->amgr->am_ofile->ofileread(&plblock, state->file, ob_blkno);
			list[level*state->bucketCapacity + offset] = plblock;
		}
		offset = 0;
	}

	return list;
}

void addBlocksToStash(ORAMState state*, PLBList list){

	size_t status = 0;
	size_t index = 0;

	for(index; index < state->treeHeight*state->bucketCapacity; index++){
		if(list[index]->blokno != DUMMY_BLOCK){
			status = state->amgr->am_stash->stashadd(state->file, list[index]);
		}
	}
}

/***
 *
 * a_leaf -> leaf of accessed offset
 *
 */
size_t getBlocksToWrite(PLBlock *blocksToWrite, size_t a_leaf, ORAMState state){

	size_t total = 0;
	// Index to keep track of current tree level
	size_t level = state->treeHeight;
	//Leaf number of a stash node
	size_t s_leaf = 0;
	size_t padding = 0;
	PLBlock pl_block;
	PLBList selectedBlocks;
	PLBlock r_block;
	void* randomBlock;
	size_t save_errno = 0;
	
	initBlockList(state, &selectedBlocks);

	for(level; level > 0; level--){
		status = state->amgr->am_stash->stashstartIt(state->file);
		
		while(state->amgr->am_stash->stashnext(state->file, &pl_block) 
			  && total < state->bucketCapacity){

			s_leaf = state->am_pmap->pmget(state->file, pl_block->blkno);

			if(check(a_leaf, s_leaf, level)){
				selectedBlocks[level*state->bucketCapacity + total] = pl_block;
				total++;
			}
		}

		padding = total;

		//Add missing
		save_errno = errno;
		errno = 0;

		for(padding; padding < state->bucketCapacity; padding++){
			r_block = (PLBLock) malloc(sizeof(PLBLock));
			
			if(r_block == NULL && errno = ENOMEM){
				elog(OUT_OF_MEMORY);
				errno = save_errno;
				abort();
			}
			
			randomBlock = malloc(sizeof(char)*state->blockSize);

			if(randomblock == NULL && errno = ENOMEM){
				elog(OUT_OF_MEMORY);
				errno = save_errno;
				abort();
			}


			r_block->blkno = -1;
			r_block->block = randomblock;
			selectedBlocks[level*state->bucketCapacity + padding] = r_block;

		}
		total = 0;
		state->amgr->am_stash->stashcloseIt();

	}
	errno = save_errno;

	*blocksToWrite = selectedBlocks;

}


void writeBlocksToStorage(PLBList list, size_t leaf, ORAMState state){
	size_t list_offset = 0;
	size_t bucketOffset = 0;
	size_t result = 0;
	size_t index = 0;
	BlockNumber ob_blkno = 0;
	PLBLock block = NULL;

	currentPosition = leaf + (1 << state->treeHeight);//Tree Level + 1

	while(currentPosition > 0 ){
		
		for(index=0; index < state->bucketCapacity; index++){
			ob_blkno = (currentPosition-1)*state->bucketCapacity + index;
			block = list[list_offset+index];
			state->amgr->am_ofile->ofilewrite(block, state->filename, ob_blkno);
		}
		list_offset += state->bucketCapacity;
		currentPosition >>= 1;

	}
}

int check(size_t a_leaf, size_t s_leaf, size_t level){
	return (a_leaf >> level) == (s_leaf >> level)
}

void updateBlockLeaf(BlockNumber blkno, ORAMState state){
	int result = 0;
	BlockNumber r = rand()%(pow(2,state->treeHeight)-1);
	result = state->amgr->am_pmap->pmupdate(r, blkno, state->filename);
}

void updateStashWithNewBlock(void *data, BlockNumber blkno, ORAMState state){
	int result = 0;
	PLBlock block = NULL;
	block.blkno = blkno;
	block.block = block;
	resut = state->amgr->am_stash->stashadd(state->filename, block);
}

void removeBlocksFromStash(PLBList list, ORAMState state){
	size_t level = 0;
	size_t offset = 0;
	PLBlock pl_block = NULL;

	for(level; level < state->treeHeight; level++){
		for(offset; offset < state->bucketOffset; offset++){
			pl_block = list[level*state->bucketCapacity+offset];
			state->amgr->am_stash->stashremove(state->filename, pl_block);

			/**
			 * The block has been persisted to disk and removed from the 
			 * stash, thus the memory references can be freed.
			 */
			free(pl_block->block);
			free(pl_block);
		}
		offset = 0;
	}
}


void freeTreePath(TreePath path, ORAMState state){
	size_t level = 0;

	for(level; level < state->treeHeight; level++){
		free(path[level]);
	}

	free(path);


}

size_t read(void **ptr, BlockNumber blkno, ORAMState state){
	size_t leaf = 0;
	TreePath path = NULL;
	PLBList list = NULL;
	PLBList blocks_to_write = NULL;
	void* block;

	// line 1 and 2 of original paper
	leaf = state->pmap->am_pmap->pmget(state->file, blkno);
	updateBlockLeaf(blkno,state);

	// line 3 to 5 of original paper
	path = getTreePath(state, leaf);
	list = getTreeNodes(state, path);
	addBlocksToStash(state, list);

	//Line 6 of original paper
	state->amgr->am_stash->stashget(&block, blkno, state->file);

	//line 10 to 15 of original paper
	getBlocksToWrite(&blocks_to_write, leaf, state);
	writeBlockstoStorage(blocks_to_write, leaf, state);
	removeBlocksFromStash(blocks_to_write, state);

	//Free Resources
	freeTreePath(path);
	/*The plblocks of the list cannot be freed as they may be in the stash*/
	free(list);

	*ptr = block;
	return state->blockSize;
}

size_t write(void *data, BlockNumber blkno, ORAMState state){
	size_t leaf = 0;
	TreePath path = NULL;
	PLBList list = NULL;
	PLBList blocks_to_write = NULL;
	size_t result = 0;

	// line 1 and 2 of original paper
	leaf = state->pmap->am_pmap->pmget(state->file, blkno);
	updateBlockLeaf(blkno,state);

	// line 3 to 5 of original paper
	path = getTreePath(state, leaf);
	list = getTreeNodes(state, path);
	addBlocksToStash(state, list);

	// line 7 to 9 of original paper
	updateStashWithNewBlock(data, blkno, state);

	//line 10 to 15 of original paper
	getBlocksToWrite(&blocks_to_write, leaf, state);
	writeBlockstoStorage(blocks_to_write, leaf, state);
	removeBlocksFromStash(blocks_to_write, state);

	//Free Resources
	freeTreePath(path);
	/*The plblocks of the list cannot be freed as they may be in the stash*/
	free(list);

	return state->blockSize;
}