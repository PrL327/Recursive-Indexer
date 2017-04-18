#include <libgen.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>
#include "Indexer.h"

/*
*   Function Declarations in Order as they appear
*/
int dirCheck(char*);
int Read_File(char*, char*);
int Write_File(char*);

static char* IndexFileName; //used to hold the name of the file to write the xml to

/*
 * Function: dirMain
 *-----------------------
 *   First checks if the file to print to already exists (if it does it asks user to overwrite it)
 *   If user says yes or it didn't exist, then it continues going through the directory or file given
 *   If user says no, then the program ends since we need to overwrite the file to continue
 *
 *   Called by: main()
 *
 *   printFileName: file to print xml to
 *   dirPath: path to directory or file to index
 *
 *   returns: 0 (if its successful), -1 if something fails
 */
int dirMain(char* printFileName, char* dirPath)
{
    IndexFileName = printFileName; //makes global variable IndexFileName hold name of file to print to
    FILE *file = fopen(printFileName, "r"); //tries to open file to check if it exists

    char overwrite = '0'; //initializes char that holds user input

    if(file) //if the file exists then ask user for permission to overwrite it
    {
        int cannotOpen = access(IndexFileName, W_OK); //get access permission
        if(cannotOpen)
        {
            printf("Don't have access to overwrite file, closing program...\n");
            return -1;
        }
        printf("Do you want to overwrite it? [Y/N]: "); //asks the user the question
        scanf("%c", &overwrite); //gets user response
        while(tolower(overwrite) != 'y' && tolower(overwrite) != 'n') //keeps asking until user answers yes or no
        {
            printf("Do you want to overwrite it? [Y/N]: ");//asks the user the question
            scanf("%c", &overwrite);//gets user response
        }
        if(tolower(overwrite) == 'y') //if permission to overwrite
        {
            fclose(file); //close file
            file = fopen(printFileName, "w"); //clear its contents
            fclose(file); //close file again
        }
        else
        {
            printf("Unable to continue, closing program...\n"); //tells user we need to overwrite file
            return -1; //tells us the program failed
        }
    }
    int wentWrong = dirCheck(dirPath); //gets value returned from dirCheck and its recursions
    if(!wentWrong) //if everything went well
    {
        printList(); //print all data from indexer
    }
    return wentWrong; //return wentWrong
}

/*
 * Function: dirCheck
 *-----------------------
 *   checks whether the initial path given is a file or directory
 *   if its a file, we simply open and get its contents
 *   if its a directory, we go through its contents and if we find a file,
 *   we read it and add its contents to the indexer, if its a directory, then
 *   we go through that directory until we hit the end
 *
 *   Called by: dirmain()
 *
 *   path: path to directory or file to index
 *
 *   returns: 0 (if its successful), -1 if something fails
 */
int dirCheck(char *path)
{
    if(strlen(path) > PATH_MAX) //if file path is too long for OS to handle, abort the process
    {
        printf("Path is too long to handle, closing program down...\n"); //tell user why we cannot continue
        return -1; //tell user program failed
    }
    DIR *dr = opendir(path); //attempt to open the directory
    char *filepath = NULL; //used to hold path to a file when we find a regular file
    int status = 0; //status of dirCheck and its children
    if(dr) //if its a directory
    {
       struct dirent *drread = NULL; //struct to hold return value of readdir()
       while((drread = readdir(dr)) != NULL && status != -1) //get next object in directory and make sure none of the children failed
       {
            if((drread -> d_type) == DT_REG) //if its a regular file
            {
                filepath = calloc(strlen(path)+strlen(drread -> d_name) + 3, sizeof(char)); //create memory to hold file path
                if(!filepath) //if calloc failed
                {
                    printf("unable to allocate memory for file path, closing down...\n");
                    return -1;
                }
                sprintf(filepath,"%s/%s",path,(drread -> d_name)); //create the file path
                printf("Found file: %s\n", (drread -> d_name)); //tell user we found a file
                Read_File(filepath, (drread -> d_name)); //read the file
                free(filepath); //free the memory
                filepath = NULL; //set filepath back to null
            }
            else if((drread -> d_type) == DT_DIR) //if its a directory
            {
                if(strcmp((drread -> d_name), ".")!=0 && strcmp((drread -> d_name), "..")!=0) //if its not parent or root directory
                {
                    filepath = calloc(strlen(path)+strlen(drread -> d_name) + 3, sizeof(char)); //create memory to hold directory path
                    if(!filepath) //if it failed
                    {
                         printf("unable to allocate memory for directory path, closing down...\n");
                        return -1;
                    }
                    sprintf(filepath,"%s/%s",path,(drread -> d_name)); //create directory path with current directory
                    printf("Found directory: %s\n", filepath); //tell user what directory we found
                    status = dirCheck(filepath); //go through its directory and get its status
                    free(filepath); //free the memory
                    filepath = NULL; //set pointer to null
                }
            }
            else //if its not a directory or regular file
            {
                printf("Irregular file: %s found! skipping file...\n",(drread -> d_name)); //tell user we found it but skipped it
            }
       }
       closedir(dr); //close the directory
    }
    else if(!dr) //if not a directory
    {
        FILE* file = fopen(path,"r"); //check if its a file
        if(file) //if it is
        {
            Read_File(path,basename(path));//read the file
            fclose(file);//close the file
        }

        if(!file) //if its not a file or directory
        {
            printf("Error: No File or Directory Found!\n"); //tell user what happened
            return -1; //tell user program failed
        }
    }
    return status; //return status
}
/*
 * Function: Read_File
 *-----------------------
 *   reads files and get its contents if possible
 *
 *   Called by: dirCheck()
 *
 *   filePath: path to file to index
 *   name: name of the file
 *
 *   returns: 0 (if its successful), -1 if something fails
 */
int Read_File(char *filePath, char *name)
{
    int cannotOpen = access(filePath, R_OK); //check access
    if(cannotOpen) //dont have access to file
    {
        printf("Don't have permission to read file: %s\nSkipping file...\n",filePath);
    }

    if(!cannotOpen) //can access file
    {
        char* buffer = NULL; //holds the file contents
        int file = open(filePath, O_RDONLY); //opens the file for reading
        int length = 0; //length of the file

        if(file >= 0)
        {
            FILE* tempFile = fopen(filePath, "r"); //open file to get its length
            fseek (tempFile, 0, SEEK_END); //set seek to end of file
            length = ftell(tempFile); //get length of file
            fseek(tempFile, 0, SEEK_SET); //reset seek
            fclose(tempFile); //close the file
            buffer = malloc (length+1); //get memory to hold the file contents
            short hitEof = 0; //boolean for when we hit the eof
            if (buffer)
            {
                int status = 0; //how much bytes we read in
                int readin = 0; //total bytes we read in, also used to add characters to pointer
                while(readin < length && !hitEof)
                {
                    status = read(file, buffer+readin, length-readin); //get how much bytes we read
                    if(status > 0) //if we didnt hit end of file,
                    {
                        readin += status; //increment readin by status
                    }
                    else if (status == 0) //if we hit eof
                    {
                        hitEof = 1; //tells us we hit the eof
                    }
                    else //something went wrong
                    {
                        printf("Error reading in file\n");
                        perror("Error");
                        return -1;
                    }
                }
                buffer[readin] = '\0'; //null terminate the buffer
            }
            stringSplitter(buffer, name); //send the file name and its contents to be indexed
            if(buffer) //if we had memory for buffer
            {
               free(buffer); //free buffer memory
            }
            close (file); //close the file
        }
        buffer = NULL; //set buffer to NULL
    }
    return 0;
}

/*
 * Function: Write_File
 *-----------------------
 *   writes the xml to the file specified
 *
 *   Called by: printList()
 *
 *   fileStr: string to print to file
 *
 *   returns: 0 (if its successful), -1 if something fails
 */
int Write_File(char* fileStr)
{
    FILE *checkFile = fopen(IndexFileName, "r"); //open the file
    int cannotOpen = 0; //used to check access
    if(checkFile) //if file exists
    {
        cannotOpen = access(IndexFileName, W_OK); //get access permission
        fclose(checkFile); //close the file
    }

    if(cannotOpen) //if we dont have write access
    {
        printf("Don't have permission to write in file: %s\nSkipping file...\n",IndexFileName);
        return -1;
    }
    else //have permission or file didnt exist
    {
        FILE* file = fopen(IndexFileName,"a"); //open file for appending
        if(file) //if successful
        {
            fwrite(fileStr,strlen(fileStr),1,file); //write contents to the file
            fclose(file); //close the file
        }
    }
    return 0; //everything went well
}
