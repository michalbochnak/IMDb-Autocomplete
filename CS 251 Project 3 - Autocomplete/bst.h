/*bst.h*/

//
// Binary Search Tree ADT header file.
//
// << Michal Bochnak >>
// U. of Illinois, Chicago
// CS251, Spring 2017
// Project 03
//

// make sure this header file is #include exactly once:
#pragma once


//
// BST type declarations:
//
typedef char*  BSTKey;
typedef struct BSTValue
{
	long long weight;
} BSTValue;

typedef struct BSTNode
{
	BSTKey    Key;
	BSTValue  Value;
	struct BSTNode  *Left;
	struct BSTNode  *Right;
} BSTNode;

typedef struct BST
{
	BSTNode *Root;
	int      Count;
} BST;


//
// BST API: function prototypes
//
BST *BSTCreate();
BST *BSTBuildTree(BST *tree, FILE *pInputFile);
int  BSTCompareKeys(BSTKey key1, BSTKey key2);
BSTNode *BSTSearch(BST *tree, BSTKey key);
BSTNode* BSTSearchPrefix(BST *tree, char *prefix);
int  BSTInsert(BST *tree, BSTKey key, BSTValue value);
int       BSTHeight(BST *tree);
int BSTCountSubtree(BSTNode *root);
void BSTCountMatches(BSTNode *root, char *prefix, int *matches);
void BSTInsertInArray(BSTNode *root, char *prefix, BSTNode *matched, int *index);