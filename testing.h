#ifndef TESTING_H_INCLUDED
#define TESTING_H_INCLUDED

#define malloc( x ) callMalloc( x, __FILE__, __LINE__ )

int stringSplitter(char*, char*);

#endif // TESTING_H_INCLUDED
