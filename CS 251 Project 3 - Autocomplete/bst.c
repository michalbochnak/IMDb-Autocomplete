/*bst.c*/

//
// Binary Search Tree ADT implementation file.
//
// << Michal Bochnak >>
// U. of Illinois, Chicago
// CS251, Spring 2017
// Project 03
//

// ignore stdlib warnings if working in Visual Studio:
#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "bst.h"


//
// BSTCreate: dynamically creates and returns an empty binary search tree:
//
BST *BSTCreate() {
	BST *tree;		// handle to the tree

	// malloc the memory for the tree handle
	tree = (BST *)malloc(sizeof(BST));
	tree->Root = NULL;
	tree->Count = 0;

	return tree;
}


//
//	BSTBuildTree: builds tree using the input file
//
BST *BSTBuildTree(BST *tree, FILE *pInputFile) {
	char line[512];	// string read in from the file
	BSTValue value;
	char *token;	// used to extract number from the beggining of the line
	char *cp;		// used to skip whitespace characters in string

	while (fgets(line, 512, pInputFile) != NULL) {	// while not end of the file
		line[strcspn(line, "\r\n")] = '\0';	// avoid errors on different platforms
	
		// extract the number part of the string and convert to number
		token = strtok(line, " \t");
		value.weight = atoi(token);
	
		// skip whitespace characters before the string which will be used as key
		cp = strtok(NULL, "\0");
		while (*cp == ' ' || *cp == '\t') {
			cp++;
		}

		BSTInsert(tree, cp, value);	// insert into tree
	}
	return tree;
}


//
// BSTCompareKeys: compares key1 and key2, returning
//   value < 0 if key1 <  key2
//   0         if key1 == key2
//   value > 0 if key1 >  key2
//
int BSTCompareKeys(BSTKey key1, BSTKey key2) {
	if (strcmp(key1, key2) < 0)
		return -1;
	else if (strcmp(key1, key2) == 0)
		return 0;
	else
		return 1;
}


//
// BSTSearch: searches the binary search tree for a node containing the
// same key.  If a match is found, a pointer to the node is returned, 
// otherwise NULL is returned.
//
BSTNode *BSTSearch(BST *tree, BSTKey key) {
	BSTNode *cur = tree->Root;

	//
	// search the tree to see if it contains a matching key:
	//
	while (cur != NULL) 	{
		if (BSTCompareKeys(key, cur->Key) == 0)  // found!
			return cur;
		else if (BSTCompareKeys(key, cur->Key) < 0) { // smaller, go left:
			cur = cur->Left;
		}
		else { // larger, go right:
			cur = cur->Right;
		}
	}

	// if we get here, we fell out of the tree, and didn't find it:
	return NULL;
}


//
// BSTSearchPrefix: searches for the sub-root for the node containing
// the prefix and returns sub-root if found, otherwise return NULL
//
BSTNode *BSTSearchPrefix(BST *tree, char *prefix) {
	BSTNode *cur = tree->Root;
	// malloc the memory for the part of the "key",
	// which is length of: prefix + ( 1 for NULL char )
	char *keyPart = (char*)malloc((sizeof(char)*(strlen(prefix)))+1);
	int i = 0;		// loop counter

	while (cur != NULL) {	
		// copy desired char amount from node key into keyPart
		for (i = 0; i < strlen(prefix); i++) {
			keyPart[i] = cur->Key[i];
		}
		keyPart[i] = '\0';		// attach NULL at the end of keyPart
		if (BSTCompareKeys(prefix, keyPart) == 0)		// found!
			return cur;
		else if (BSTCompareKeys(prefix, keyPart) < 0)	// smaller, go left:
			cur = cur->Left;
		else // larger, go right:
			cur = cur->Right;
	}

	return NULL;	// not found, fell out of the tree
}


//
// BSTInsert: inserts the given (key, value) pair into the binary search
// tree.  Returns true (non-zero) if the insert was successful, returns 
// false (0) if the given key is already in the tree -- in which case the
// given (key, value) pair is not inserted.
//
int BSTInsert(BST *tree, BSTKey key, BSTValue value) {
	BSTNode *prev = NULL;
	BSTNode *cur = tree->Root;

	//
	// first we search the tree to see if it already contains key:
	//
	while (cur != NULL) {
		if (BSTCompareKeys(key, cur->Key) == 0)  // already in tree, failed:
			return 0;
		else if (BSTCompareKeys(key, cur->Key) < 0) { // smaller, go left:
			prev = cur;
			cur = cur->Left;
		}
		else { // larger, go right:
			prev = cur;
			cur = cur->Right;
		}
	}

	// 
	// If we get here, tree does not contain key, so insert new node
	// where we fell out of tree:
	//
	BSTNode *T = (BSTNode *)malloc(sizeof(BSTNode));
	T->Key = (char*)malloc((sizeof(char) * (strlen(key)))+1);
	strcpy(T->Key, key);
	T->Value = value;
	T->Left = NULL;
	T->Right = NULL;

	//
	// link T where we fell out of tree -- after prev:
	//

	if (prev == NULL)  // tree is empty, insert @ root:
		tree->Root = T;
	else if (BSTCompareKeys(key, prev->Key) < 0)  // smaller, insert to left:
		prev->Left = T;
	else  // larger, insert to right:
		prev->Right = T;

	tree->Count++;
	return 1;  // success:
}


//
// BSTHeight: returns the height of the tree.
//
int _max2(int x, int y) {
	// if ( x > y ) returns x, otherwise returns y
	return (x > y) ? x : y;
}

int _BSTHeight(BSTNode *root) {
	if (root == NULL)
		return -1;		// empty, -1 as a height is returned
	else	// return 1 + height of bigger sub-tree
		return 1 + _max2(_BSTHeight(root->Left), _BSTHeight(root->Right));
}

int BSTHeight(BST *tree) {
	return _BSTHeight(tree->Root);
}


//
// CountSubtree: counts the nodes in sub-tree
//
int BSTCountSubtree(BSTNode *root) {
	if (root == NULL) 
		return 0;	// empty, return 0
	else	// return 1 + number of nodes in left and right sub-tree
		return 1 + BSTCountSubtree(root->Left) + BSTCountSubtree(root->Right);
}


//
// CountMatches: counts the nodes in the tree
// which key's contains given prefix
//
void BSTCountMatches(BSTNode *root, char *prefix, int *M) {
	BSTNode *cur = root;
	int i = 0;	// loop counter

	// base case
	if (root == NULL)
		return;		// empty, so return
	
	BSTCountMatches(cur->Left, prefix, *&M);	// count matches in left sub-tree

	// malloc the memory for the part of the "key",
	// which is length of: prefix + ( 1 for NULL char )
	char *keyPart = (char*)malloc((sizeof(char)*(strlen(prefix))) + 1);	
	
	// copy desired char amount from node into keyPart
	for (i = 0; i < strlen(prefix); i++)
		keyPart[i] = cur->Key[i];	
	keyPart[i] = '\0';		// attach NULL at the end of keyPart

	if (BSTCompareKeys(prefix, keyPart) == 0)  // found!
		(*M)++;		// increment M
	
	BSTCountMatches(cur->Right, prefix, *&M);	// count matches in right sub-tree
}


//
// InsertInArray: inserts the nodes which key's contains given prefix into the array
//
void BSTInsertInArray(BSTNode *root, char *prefix, BSTNode *matched, int *index) {
	int i = 0;	// loop counter

	// base case
	if (root == NULL)
		return;
	else {
		// traverse left sub-tree
		BSTInsertInArray(root->Left, *&prefix, *&matched, *&index);

		// malloc the memory for the part of the "key",
		// which is length of: prefix + ( 1 for NULL char )
		char *keyPart = (char*)malloc((sizeof(char)*(strlen(prefix))) + 1);	// +1 for NULL char

		// copy desired char amount from node into keyPart
		for (i = 0; i < strlen(prefix); i++) 
			keyPart[i] = root->Key[i];
		keyPart[i] = '\0';		// attach NULL at the end of keyPart

		if (BSTCompareKeys(prefix, keyPart) == 0) { // found, insert into array
			matched[*index].Value = root->Value;
			matched[*index].Key = (BSTKey*)malloc((sizeof(BSTKey) * strlen(root->Key)) + 1);
			strcpy(matched[*index].Key, root->Key);
			*index = *index + 1;
		}

		// traverse right sub-tree
		BSTInsertInArray(root->Right, *&prefix, *&matched, *&index);
	}
	return;		// done
}