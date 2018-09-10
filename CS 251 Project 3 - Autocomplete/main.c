/*main.cpp*/

//
// Auto-complete using a binary search tree.
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
#include <ctype.h>

#include "bst.h"


//
//	Functions declarations
//
void skipRestOfInput(FILE *stream);
char *getFileName();
void DisplaySuggestions(BSTNode *matched, int k);
void SelectionSort(BSTNode *matches, int numOfElem);




int main()
{
	char *filename;
	char  cmd[64];
	char  text[512];
	int   textsize = sizeof(text) / sizeof(text[0]);
	BST	  *tree;			// handle to the binary search tree
	FILE  *pInputFile;		// pointer to the input file

	printf("** Starting Autocomplete **\n");

	filename = getFileName();
	
	pInputFile = fopen(filename, "r");	// open the file

	// create the tree
	tree = BSTCreate();

	// build the tree by using input file
	BSTBuildTree(tree, pInputFile);

	//
	// now interact with user:
	//
	printf("** Ready **\n");

	scanf("%s", cmd);		// read the command in

	while (strcmp(cmd, "exit") != 0) {
		if (strcmp(cmd, "stats") == 0) {	
			// display stats about tree
			printf("**Tree count:  %d\n", tree->Count);
			printf("**Tree Height: %d\n", BSTHeight(tree));
		}
		else if (strcmp(cmd, "add") == 0) {	
			// add the new element to the tree if element is not there yet
			long long weight;		// weight to add
			char      part2[512];	// part 2 of input string
			int       part2size = sizeof(part2) / sizeof(part2[0]);
			int		  status = 0;	// status of insertion, 1 - inserted, otherwise not
			BSTValue  value;		// value to insert

			//
			// add weight text
			//
			scanf("%lld %s", &weight, text);	// read user input in
			fgets(part2, part2size, stdin);		// read the rest of the string
			part2[strcspn(part2, "\r\n")] = '\0';  // strip EOL char(s):
			strcat(text, part2);		// attach strings together
			value.weight = weight;		// store weight into value struct
			status = BSTInsert(tree, text, value);	// try to insert new node
			if (status == 1)			// added succesfully
				printf("**Added\n");
			else						// already in the tree, not added
				printf("**Not added...\n");
		}
		else if (strcmp(cmd, "find") == 0) {
			char part2[512];			// part 2 of input string
			int  part2size = sizeof(part2) / sizeof(part2[0]);
			// malloc memory for the node to add
			BSTNode *found = (BSTNode*)malloc(sizeof(BSTNode));

			//
			// find text
			//
			scanf("%s", text);					// read user input in
			fgets(part2, part2size, stdin);		// read the rest of the string
			part2[strcspn(part2, "\r\n")] = '\0';  // strip EOL char(s):
			strcat(text, part2);				// attach strings together

			found = BSTSearch(tree, text);		// search the tree
			if (found != NULL)					// found, display node that was found
				printf("%s: %lld\n", found->Key, found->Value.weight);
			else								// not found	
				printf("**Not found...\n");
		}
		else if (strcmp(cmd, "suggest") == 0) {
			int  k;					// number of suggestions to display
			char part2[512];		// part 2 of input string
			int  part2size = sizeof(part2) / sizeof(part2[0]);
			// malloc memory for the sub-root
			BSTNode *R = (BSTNode*)malloc(sizeof(BSTNode));
			int M = 0;				// number of matches in the subtree
			BSTNode *matched;		// array of matched suggestions
			int index = 0;			// index in the array

			//
			// suggest k text
			//
			scanf("%d %s", &k, text);		// read user input in
			fgets(part2, part2size, stdin);	// read the rest of the string
			part2[strcspn(part2, "\r\n")] = '\0';  // strip EOL char(s):
			strcat(text, part2);			// attach strings together

			// search for sub-root which matches user prefix, assign to R
			R = BSTSearchPrefix(tree, text);

			if (R != NULL) {	// R != NULL, sub-root found, matching suggestions exist
				// count the matches in the tree and store the result in M
				BSTCountMatches(R, text, &M);	

				// display stats about matches found in the tree
				printf("** [Sub-tree root:  (%s,%lld)]\n", R->Key, R->Value.weight);
				printf("** [Sub-tree count: %d]\n", BSTCountSubtree(R));
				printf("** [Num matches:    %d]\n", M);
				// malloc the memory for the matched suggestions
				matched = (BSTNode*)malloc(sizeof(BSTNode) * M);		
				BSTInsertInArray(R, text, matched, &index);	// insert matching nodes into the array
				SelectionSort(matched, M);					// sort the array with elements
				if (k > M)	// less suggestions exist, then requested from user
					k = M;	// assign number of suggestions ( M ) to number of suggestions to display ( k )
				DisplaySuggestions(matched, k);
			}
			else		// R == NULL, no suggestions exist
				printf("**No suggestions...\n");
		}
		else {		// incorect command entered
			printf("**unknown cmd, try again...\n");
		}

		scanf("%s", cmd);	// read in next command
	}

	printf("** Done **\n");	

	return 0;
}	// end of main




// ----------------------------------------------------------------------------
//	Functions definitions
// ----------------------------------------------------------------------------

//
// skipRestOfInput:
//
// Inputs and discards the remainder of the current line for the 
// given input stream, including the EOL character(s).
//
void skipRestOfInput(FILE *stream) {
	char restOfLine[256];
	int rolLength = sizeof(restOfLine) / sizeof(restOfLine[0]);

	fgets(restOfLine, rolLength, stream);
}

//
// getFileName: inputs a filename from the keyboard, make sure the file can be
// opened, and returns the filename if so.  If the file cannot be opened, an
// error message is output and the program is exited.
//
char *getFileName() {
	char filename[512];
	int  fnsize = sizeof(filename) / sizeof(filename[0]);

	// input filename from the keyboard:
	fgets(filename, fnsize, stdin);
	filename[strcspn(filename, "\r\n")] = '\0';  // strip EOL char(s):

												 // make sure filename exists and can be opened:
	FILE *infile = fopen(filename, "r");
	if (infile == NULL)
	{
		printf("**Error: unable to open '%s'\n\n", filename);
		exit(-1);
	}

	fclose(infile);

	// duplicate and return filename:
	char *s = (char *)malloc((strlen(filename) + 1) * sizeof(char));
	strcpy(s, filename);

	return s;
}


//
// DisplaySuggestions: displays suggestions
//
void DisplaySuggestions(BSTNode *matched, int k) {
	int i = 0;		// loop counter

	for (i = 0; i < k; i++) {
		printf("%s: %lld\n", matched[i].Key, matched[i].Value.weight);
	}
}


//
// SelectionSort: sorts the array of BSTNode structs
//
void SelectionSort(BSTNode *matches, int numOfElem) {
	int i = 0;					// outer loop counter
	int j = 0;					// inner loop counter
	int maxIndex = 0;			// stores index of minimum value during selection sort
	long long tempVal = 0;		// copy for swapping
	char tempKey[512];			// copy for swapping

	for (i = 0; i < numOfElem; i++) {
		maxIndex = i;			// assign current i for maxIndex
		for (j = i + 1; j < numOfElem; j++) {
			if (matches[j].Value.weight > matches[maxIndex].Value.weight) {
				maxIndex = j;	// new maxIndex found, assign to maxIndex
			}
		}

		// swap the elements in the array
		tempVal = matches[i].Value.weight;	// store weight into tempVal
		strcpy(tempKey, matches[i].Key);	// store key into tempKey

		// move weight and key of [maxIndex] into struct of [i] index
		matches[i].Value.weight = matches[maxIndex].Value.weight;
		strcpy(matches[i].Key, matches[maxIndex].Key);

		// move weight from tempVal to struct of [maxIndex]
		matches[maxIndex].Value.weight = tempVal;
		// move key from tempKey to struct of [maxIndex]
		strcpy(matches[maxIndex].Key, tempKey);
	}
}