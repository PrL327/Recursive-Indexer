#ifndef INDEXER_H_INCLUDED
#define INDEXER_H_INCLUDED
#include <stddef.h>

//static WordText *head = NULL;

int stringSplitter(char*, char*);
int dirMain(char*, char*);
void* callMalloc(int, char*, int);
void printList();
int Write_File(char*);

#endif // INDEXER_H_INCLUDED
