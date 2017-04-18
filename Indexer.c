#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Indexer.h"


typedef struct
{
    char * fileName; //holds the name of the file
    size_t frequency; //holds how many times the word appears in this file
    struct FileCount * nextFile; //points to the next file in the linked list
}FileCount;

/*
 *the Nodes that will make up the linked list
 */
typedef struct
{
	char * word; //designed to hold the string
    FileCount * fileNames; //file names with their count
	struct WordText * next; //connects the node to another
}WordText;

/*
*   Function Declarations in Order as they appear
*/
int minimum(char*, char*);
int stringCompare(char* , char*);
int addWord(WordText**, char*, char*);
int addFile(WordText*, char*);
void printFile(FileCount* f);
void clearList(WordText**);
void clearFiles(FileCount**, char*);
void errorMessage(int, char*, int);
void stringToLower(char*);
int numberOfDigits(int n);

static WordText * firstWord = NULL; //holds the first wordText (aka the head node)

/*
 * Function: stringSplitter
 *-----------------------
 *   Takes in a string (Currently takes in 1st argument) and
 *   splits the string into substrings that only contain alphanumerical
 *   characters
 *
 *   Called by: main() (line 41)
 *
 *   fileInput: the string we will be splitting
 *
 *   returns: 0 (if its successful), -1 if something fails
 */
int stringSplitter(char* fileInput, char* fileName)
{
    int errorCode = 0; //the value we return to main to tell if program ran successfully or not
    char* tempInput = calloc(strlen(fileInput) + 1, sizeof (char)); //create memory to hold file contents
    if(tempInput) //if calloc worked
    {
        strncpy(tempInput,fileInput,strlen(fileInput)); //copies the tempInput string
        tempInput[strlen(fileInput)] = '\0'; //null terminates the tempInput
        char* input = tempInput;
        char* tempString = NULL; //design to hold the new string temporarily until it can be passed to a node
        int position = 0; //we use to know how far down the string we are
        int start = 0; //we use to keep track of the beginning of the string
        int stop = -1; //we use to keep track the end of the string
        int length = 0; //length of the string

        while (*input != '\0' && errorCode == 0) //keep going until either we hit the end of the string or code down the line fails
        {
            if (isalpha(*input)) //is c an alphabetic character
            {
                start = position; //point to where the new string starts
                position++; //move the position along
                input++; //move input along the string
                while (isalnum(*input)) //keeps adding to the string if there alphabetical characters
                {
                    input++; //move input along the string
                    position++; //move the position along
                }
                stop = position; //tells us where the string ends
                length = stop - start; //get the length of the string
                tempString = malloc(sizeof(char)*(length+1)); //allocate memory and add a byte for '\0'
                if(!tempString) //if malloc() failed return errorCode and message
                {
                    printf("malloc failed on line 83");
                    return -1;
                }
                else //if it worked, go ahead and add new string to the list
                {
                    strncpy(tempString,input - length, length); //copy the new string to the string variable
                    tempString[length] = '\0'; //null terminate the new string
                    errorCode = addWord(&firstWord, tempString, fileName); //add the node and return an error code if any
                    free(tempString); //free string since we don't need it anymore
                }
            }
            else //else move onto the next
            {
                position++; //move the position by one
                input++; //move input cursor down the string
            }
        }
        if(stop == -1) //tell user there were no strings to find
        {
            printf("Warning: No strings found in %s!\n", fileName);
        }
        free(tempInput);
        input = NULL;
    }
    else
    {
         printf("calloc failed on line 57");
    }
    return errorCode; //tell main if program is successful or not
}


/*
* Function: minimum
*-----------------------
*   Finds which string is the shortest in length
*
*   Called by: strCompare() (line 154)
*
*   nodeString: the string from the linked list being compared
*   newString: the new string to be added to the linked list
*
*   returns: the length of smallest string
*/
int minimum(char * nodeString, char * newString)
{
	int lengthOfnodeString = strlen(nodeString); //length of the node string
	int lengthOfnewString = strlen(newString); //length of the new string
	if (lengthOfnodeString < lengthOfnewString) //if the node string has less characters
		return lengthOfnodeString;
	else //if the new String is shorter than the node string
		return lengthOfnewString;
}

/*
* Function: stringCompare - might need work
*-----------------------
*  compares two strings by first
*  getting the length of the smaller
*  of the two strings, then compares
*  the strings to min bytes. If the 2 strings have the same min bytes
*  then the function chooses which string is the shortest
*
*   Called by: addNode() (line 209)
*
*   nodeString: the string from the linked list being compared
*   newString: the new string to be added to the linked list
*
*   returns: -1 (string1 is after string2), 1 (string1 is before string2, 0 if both are the same
*/
int stringCompare(char * nodeString, char * newString)
{
	int min = minimum(nodeString, newString);//get least string length for comparison
	int i = 0;

	for(i = 0; i < min; i++)
    {
        if(isalpha(nodeString[i]) && isdigit(newString[i]))
        {
            return -1;//new string after current string
        }
        else if (isdigit(nodeString[i]) && isalpha(newString[i]))
        {
            return 1;//new string before current string
        }
        else if(isalpha(nodeString[i]) && isalpha(newString[i])) //both are letters
        {
            if(nodeString[i] < newString[i]) //if node string is before new string
            {
                return -1;
            }
            else if(nodeString[i] > newString[i])//if new string is before node string
            {
                return 1;
            }
        }
        else //both are numbers
        {
            if(nodeString[i] < newString[i])//if node string is before new string
            {
                return -1;
            }
            else if(nodeString[i] > newString[i])//if new string is before node string
            {
                return 1;
            }
        }
    }
	int compareNum = strncmp(nodeString, newString, min);//compare the strings up to min characters

	if (compareNum == 0) //if the strings share the same n bytes
		if (strlen(nodeString) < strlen(newString))
			return -1;//nodeString goes before newString
		else if (strlen(nodeString) > strlen(newString))
			return 1; //newString goes before nodeString
        else
            return 0; //strings are the same
	else
		return compareNum; //return the comparison value
}

/*
* Function: addWord
*-----------------------
*    Adds a new string to the linked list
*    it checks if the list is empty first
*    otherwise, it searches through the list to
*    to find which node to put it after
*
*    Called by: stringSplitter()
*
*    head: the beginning of the linked list
*    input: the string to add to the linked list
*    fileNameMixed: name of file (characters are mixed cases)
*
*    returns: 0 if everything went smoothly, -1 if something went wrong
*/
int addWord(WordText**head, char* inputMixed, char* fileNameMixed)
{
    char fileName[256] = ""; //initialize array to hold lower case of fileName
    strncpy(fileName,fileNameMixed,strlen(fileNameMixed)); //copy string to fileName from fileNameMixed
    stringToLower(fileName); //change the string to all lowercase
    fileName[strlen(fileNameMixed)] = '\0'; //terminate the string

    char* input = (char*) malloc(strlen(inputMixed) + 1);
    if(!input) //if input failed
    {
        printf("malloc failed on line 230");
        return -1;
    }
    strncpy(input, inputMixed, strlen(inputMixed));
    input[strlen(inputMixed)] = '\0';
    stringToLower(input);
	int length = strlen(input)+1; //length of the input string plus one for null terminator

	if (!(*head)) //if head is null
	{
		//allocates memory space for the head Node
		*head = (WordText*)malloc(sizeof(WordText)); //allocate memory for head
		if(!(*head)) //check if malloc() allocated memory for head
        {
            free(input);
            printf("malloc failed on line 244");
            return -1; //tell user malloc() failed
        }
		(*head)->word = malloc(length);
		if(!((*head)->word))
        {
            printf("malloc failed on line 251");
            free(input); //free input as well
            return -1; //tell user malloc() failed
        }
		strncpy((*head)->word,input,length); //copy the string from input into the node
		(*head)->word[length-1] = '\0'; //null terminate the string
		//the head currently doesn't connect to another Node right now
		(*head)->next = NULL; //nothing to point to
		(*head)->fileNames = NULL; //no files yet
		addFile((*head), fileName); //add file to wordText
	}
	else//if head Node doesn't exist, make the new string the head
	{
		short isItPlaced = 0; //used to detected if it found a spot for the new string
		WordText * cursor = *head; //starts the cursor at the head of the linked list
		WordText * previousCursor = NULL; //will follow behind the cursor if we need to put the new in between 2 Nodes in the linked list
		while (cursor && !isItPlaced) //while the cursor is still traversing and we haven't found a spot for the new Node
		{
			if (stringCompare(cursor->word, input) == 0) //if we found
			{
			    addFile(cursor, fileName);
				isItPlaced = 2; //found the spot to place the new Node
			}
			else if (stringCompare(cursor->word, input) > 0) //if the string should be before the cursor
			{
				isItPlaced = 1; //found the spot to place the new Node
			}
			else //needs to traverse further in the linked list
			{
				previousCursor = cursor; //previous cursor now points to where cursor did
				cursor = (WordText*)cursor -> next; //cursor traverses the list
			}
		}
		if(isItPlaced != 2)
        {
            WordText * newNode; //initialize the new Node to insert in the linked list
            newNode = (WordText*)malloc(sizeof(WordText)); //allocate's memory space for the new Node
            if(!newNode) //check if malloc() failed
            {
                printf("malloc failed on line 290");
                free(input);
                input = NULL;
                return -1; //tell user malloc() failed
            }
            newNode->word = malloc(length); //allocate memory to hold the string
            if(!(newNode->word)) //checks if malloc() failed
            {
                printf("malloc failed on line 298");
                free(newNode);
                newNode = NULL;
                free(input);
                input = NULL;
                return -1; //tell the user malloc() failed
            }
            strncpy(newNode->word,input,length); //copies the string from input into the new node
            newNode->word[length-1] = '\0'; //null terminates the string
            newNode ->fileNames = NULL;
            if (cursor == *head) //the new Node will become the head Node
            {
                WordText ** newHead = head; //take a pointer of the head (pointer of a pointer)
                newNode->next = (struct WordText*)*newHead; //the new Node now points to the previous head
                *newHead = newNode; //head now points to the new Node making it first Node
            }
            else //puts the new Node
            {
                newNode->next = (struct WordText*) cursor; //the new Node now points to where the cursor is pointing to
                previousCursor->next = (struct WordText*) newNode; //the previous cursor now points to the new Node
            }
            addFile(newNode, fileName);
        }
	}
	free(input); //done with input so we freeit
	input = NULL; //set input to NULL
	return 0; //everything worked
}

/*
 * Function: addFile
 *-----------------------
 *    Adds the file to the list of files of the current word
 *
 *    Called by: addWord
 *
 *    word: the word were adding the file to
 *    fileName: the file name to add to the list
 *
 *    returns: 0 if everything went smoothly, -1 if something went wrong
*/
int addFile(WordText* word, char*newFileName)//- Working on
{
    if(!(word ->fileNames)) //if there is no file names to begin with
    {
        int nameLength = strlen(newFileName) + 1; //length of file name + one for null terminator
        FileCount * newFile = (FileCount*)malloc(sizeof(FileCount)); //initialize the new Node to insert in the linked list
		if(!newFile) //check if malloc() failed
        {
            printf("malloc failed on line 347");
            return -1; //tell user malloc() failed
        }
		newFile->fileName = malloc(nameLength); //allocate memory to hold the string
		if(!(newFile->fileName)) //checks if malloc() failed
        {
            printf("malloc failed on line 353");
            free(newFile);
            newFile = NULL;
            return -1; //tell the user malloc() failed
        }
		strncpy(newFile->fileName,newFileName,nameLength); //copies the string from input into the new node
		newFile -> fileName[nameLength-1] = '\0'; //null terminates the string
		newFile -> nextFile = NULL; //there is no next file to point to
		newFile -> frequency = 1; //there is one occurrence of
		word -> fileNames = newFile; //now word has a file to point to
    }
    else
    {
        short isItPlaced = 0; //used to tell us when we found the correct spot
        FileCount* firstFile = (FileCount*) word -> fileNames; //get first file from word
        FileCount* currentFile = (FileCount*) word -> fileNames; //starts cursor at first file
        FileCount* prevFile = NULL; //used to place the new node before the current one
        while(currentFile && !isItPlaced) //while we have not found a spot
        {
            if (stringCompare(currentFile -> fileName, newFileName) == 0) //if the string should be before the cursor
            {
                ++(currentFile -> frequency); //add 1 to the total count
                isItPlaced = 2; //tells us we need to move current file to a new spot
            }
           else if (currentFile -> frequency == 1) //if the current file has a frequency of 1
            {
                if(stringCompare(currentFile -> fileName, newFileName) > 0) //if it should come before current file
                {
                    isItPlaced = 1; //found a spot to place new file
                }
                else
                {
                    prevFile = currentFile; //previous cursor is now what cursor was
                    currentFile = (FileCount*) currentFile -> nextFile; //move cursor to next file
                }
            }
            else //needs to traverse further in the linked list
            {
                prevFile = currentFile; //previous cursor is now what cursor was
                currentFile = (FileCount*) currentFile -> nextFile; //move cursor to next file
            }
        }
        if(isItPlaced == 2) //rearrange the nodes
        {
            isItPlaced = 0; //resets boolean
            FileCount* curFileToReArrange = currentFile;
            if(prevFile) //if there is a file before we move the current file
            {
                prevFile -> nextFile = curFileToReArrange -> nextFile; //take out the current file
            }
            currentFile = (FileCount*) word -> fileNames; //starts cursor at first file
            prevFile = NULL; //used to place the new node before the current one

            while(currentFile && !isItPlaced) //while we have not found a spot to move the file
            {
                if (curFileToReArrange -> frequency > currentFile -> frequency) //if the file has a bigger frequency
                {
                    isItPlaced = 1; //found a spot to move file to
                }
                else if (curFileToReArrange -> frequency == currentFile -> frequency) //if the files have the same frequency
                {
                    if(stringCompare(currentFile -> fileName, newFileName) > 0) //if file should come before this
                    {
                        isItPlaced = 1; //found a spot to move file to
                    }
                    else //otherwise move on to the next file
                    {
                        prevFile = currentFile; //previous cursor is now what cursor was
                        currentFile = (FileCount*) currentFile -> nextFile; //move cursor to next file
                    }
                }
                else //needs to traverse further in the linked list
                {
                    prevFile = currentFile; //previous cursor is now what cursor was
                    currentFile = (FileCount*) currentFile -> nextFile; //move cursor to next file
                }
            }
            if (currentFile == firstFile) //if the file should be the new head
            {
                curFileToReArrange -> nextFile = (struct FileCount*) firstFile; //have rearranged node point to previous first node
                word->fileNames = curFileToReArrange; //point word to the rearranged node
            }
            else if (currentFile != curFileToReArrange) //otherwise insert node into the list again
            {
                prevFile -> nextFile = (struct FileCount*) curFileToReArrange; //point previous node to rearranged node
                curFileToReArrange -> nextFile = (struct FileCount*)currentFile; //point rearranged node to current node
            }
        }
        else
        {
            int nameLength = strlen(newFileName) + 1; //length of file name + one for null terminator
            FileCount * newFile = (FileCount*)malloc(sizeof(FileCount)); //initialize the new Node to insert in the linked list
            if(!newFile) //check if malloc() failed
            {
                printf("malloc failed on line 447");
                return -1; //tell user malloc() failed
            }
            newFile->fileName = malloc(nameLength); //allocate memory to hold the string
            if(!(newFile->fileName)) //checks if malloc() failed
            {
                printf("malloc failed on line 453");
                return -1; //tell the user malloc() failed
            }
            strncpy(newFile->fileName,newFileName,nameLength); //copies the string from input into the new node
            newFile->fileName[nameLength-1] = '\0'; //null terminates the string
            if (currentFile == firstFile) //the new Node will become the word Node
            {
                newFile->nextFile = (struct FileCount*) firstFile; //the new Node now points to the previous word
                word -> fileNames = (FileCount*) newFile; //word now points to the new Node making it first Node
            }
            else //puts the new Node
            {
                newFile->nextFile = (struct FileCount*)currentFile; //the new Node now points to where the cursor is pointing to
                prevFile->nextFile = (struct FileCount*)newFile; //the previous cursor now points to the new Node
            }
            newFile -> frequency = 1; //set new node's frequency to 1
        }
    }
    return 0; //return if everything was successful
}

/*
 * Function: printList - EDIT (Print to file when function is done)
 * -----------------------
 *    Prints each word then calls printFile to print its files
 *    Starts at the head node and goes down the list
 *    Clears the list after printing
 *
 *    Called by: dirMain() in DirecOpener.c
 *
 *    cursor: traverses through the linked list starting at the head
 *
 *    returns: Nothing
 */
void printList()
{
    WordText* cursor = firstWord;
    Write_File("<?xml version=\"1.0\" encoding=\"UTF-8?>\n");
    Write_File("<fileIndex>\n");
	while (cursor) //while cursor doesn't equal to NULL
	{
	    char* tempStr = calloc(strlen("\t<word text=\"%s\">\n")+strlen(cursor->word)+1,1);
	    if(tempStr)
        {
            sprintf(tempStr, "\t<word text=\"%s\">\n", cursor->word);
            Write_File(tempStr);
            free(tempStr);
            printFile(cursor->fileNames);
            Write_File("\t</word>\n");
            cursor = (WordText*) cursor->next; //moves to next Node
        }
        else
        {
            printf("Unable to calloc on line 497, closing down...");
        }
	}
	Write_File("</fileIndex>\n");
	printf("Writing to file complete!\n");
	clearList(&firstWord); //clear the entire list including head
}

/*
* Function: printFile - EDIT (Print to file when function is done)
*-----------------------
*    Prints each file along with its frequency of the word passed in
*
*    Called by: printList()
*
*    cursor: traverses through the linked list starting at the head
*
*    returns: Nothing
*/
void printFile(FileCount* cursor)
{
    while (cursor) //while cursor doesn't equal to NULL
	{
        char* tempStr = calloc(strlen("\t\t<file name=\"%s\">%d</file>\n")+strlen(cursor->fileName)+numberOfDigits(cursor->frequency) +1,1);
	    if(tempStr)
        {
            sprintf(tempStr, "\t\t<file name=\"%s\">%d</file>\n",cursor->fileName,cursor->frequency);
            Write_File(tempStr);
            free(tempStr);
            cursor = (FileCount*) cursor->nextFile; //moves to next Node
        }
        else
        {
            printf("Unable to calloc on line 532, closing down...");
        }
	}
}

/*
 * Function: clearList
 *-----------------------
 *  Clears all allocated memory given the the linked list
 *
 *  Called by: stringSplitter() (line 109)
 *
 *  head: the beginning of the linked list
 *
 *  returns: Nothing
 */
void clearList(WordText ** head)
{
	WordText * cursor = *head; //points to where head is actually pointing at
	WordText * nodeToDelete = NULL; //design to hold the node to be deleted
	while (cursor != NULL) //loops through linked list until it hits the end
	{
		nodeToDelete = cursor; //moves the cursor node to nodeTODelete so it can be cleared
		cursor = (WordText*) cursor-> next; //moves the cursor to next node in the list
		nodeToDelete -> next = NULL; //clears pointer
		clearFiles(&(nodeToDelete->fileNames), (nodeToDelete->word));
		free(nodeToDelete -> word); //clears string since memory is allocated
		free(nodeToDelete); //clears the struct itself and the memory allocated
	}
	*head = NULL; //clears the head node itself
	//printf("WordText list cleared!\n");
}

/*
 * Function: clearFiles
 *-----------------------
 *  Clears all allocated memory given the the file linked list in
 *
 *  Called by: stringSplitter() (line 109)
 *
 *  head: the beginning of the linked list
 *  word: tells us which word the file list belongs to (used for debugging purposes only)
 *
 *  returns: Nothing
 */
void clearFiles(FileCount ** head, char* word)
{
	FileCount * cursor = *head; //points to where head is actually pointing at
	FileCount * nodeToDelete = NULL; //design to hold the node to be deleted
	while (cursor != NULL) //loops through linked list until it hits the end
	{
		nodeToDelete = cursor; //moves the cursor node to nodeTODelete so it can be cleared
		cursor = (FileCount*) cursor-> nextFile; //moves the cursor to next node in the list
		nodeToDelete -> nextFile = NULL; //clears pointer
		free(nodeToDelete -> fileName); //clears string since memory is allocated
		free(nodeToDelete); //clears the struct itself and the memory allocated
	}
	*head = NULL; //clears the head node itself
	//printf("file list of %s cleared!\n", word);
}

/*
* Function: stringToLower
*-----------------------
*  makes all the characters of a given string lower case
*
*  Called by: addWord()
*
*  String: string that we want all lower case
*
*  returns: nothing (since we edit what the pointer points to)
*/
void stringToLower(char* string)
{
    unsigned int i = 0; //set i to 0
    for(i = 0; i < strlen(string); i++) //go through entire string (excludes \0)
    {
        string[i] = tolower(string[i]); //set each character to lower case if possible
    }
}

/*
 * Function: callMalloc
 *-----------------------
 *  simply gets number of digits of a given number so
 *  we know how much bytes to malloc()
 *
 *  Called by: addWord() & addFile()
 *
 *  n: number we want number of digits of
 *
 *  returns: number of digits of given number
 */
int numberOfDigits(int n)
{
    size_t digits = 1; //holds number of digits
    while(n > 9) //while n has 2 or more digits
    {
        n /= 10; //divide n by 10
        digits++; //increment digits
    }
    return digits; //return number of digits
}
