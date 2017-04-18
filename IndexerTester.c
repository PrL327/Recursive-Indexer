/* used for testing my indexer
 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "testing.h"

int randomNumber(int, int);
void generateString(char *, int ,short);
extern short done;

int main2()
{
    srand(time(NULL));
    char newName[256] = "";
    char fileString[1024] = "";
    int i = 0, j = 0;
    int num = 0;
    //generateString(newName, 20, 0);
    generateString(fileString, 100, 1);
    while(i < 6)
    {
        generateString(newName, 256, 0);
        //generateString(fileString, 100, 1);
        num = randomNumber(1,5);
        if(i == 5)
        {
            num = 1;
        }
        for(j = 0; j < num; j++)
        {
           stringSplitter(fileString, newName);
        }

        printf("file: %s\n", fileString);
        i++;
    }
    return 0;
}

int randomNumber(int min, int max)
{
    return (rand() % max) + min;
}

void generateString(char * charArray, int maxSize ,short addSpecial)
{
    char alnumChars[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    char specialChars[] = "!@#$%^&*()";
    int size = randomNumber(1,maxSize);
    int i = 0, upper = 0, special = 0;
    for(i = 0; i < size; i++)
    {
        if(addSpecial) //if we can use special characters
        {
            special = randomNumber(0,2);
            if(special)
            {
                special = randomNumber(0,strlen(specialChars));
                charArray[i] = specialChars[special];
            }
            else
            {
                upper = randomNumber(0,2);
                charArray[i] = alnumChars[randomNumber(0,strlen(alnumChars))];
                if(upper)
                {
                    charArray[i] = toupper(charArray[i]);
                }
            }
        }
        else
        {
            upper = 1;
            charArray[i] = alnumChars[randomNumber(0,strlen(alnumChars))];
            if(upper)
            {
                charArray[i] = toupper(charArray[i]);
            }
        }
    }
}
