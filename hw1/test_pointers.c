#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>



char * removeNonAlpha(char * sWord){
    for(int i = 0; i < strlen(sWord); i++){
        if(isalnum(sWord[i]) == 0){
            memmove(&sWord[i], &sWord[i + 1], strlen(sWord) - i);        
        }
    }

    return sWord;
}


int main(int argc, char *argv[])
{

    // char * fileName = "hw1-input01.txt";
    // FILE *file = fopen("hw1-input01.txt", "r");
    // char line[80];
    // char sWord[80];
    // char * value;

    // while(fgets(line, sizeof(line), file)){
    //         value = strtok(line, ",;\"?.!\'");
    //         char * word = value;
    //         printf("%s", word);
    //         break;



    // }

    FILE *ft;
    char const *name = "hw1-input02.txt";
    int ch;
    ft = fopen(name, "r+");
    if (ft == NULL)
    {
        fprintf(stderr, "cannot open target file %s\n", name);
        exit(1);
    }
    while ((ch = fgetc(ft)) != EOF)
    {
        if(isalnum(ch) == 0 && ch != ' ' && ch!= '\n'){
            printf("%c", ch);

        }
    }
    fclose(ft);




    // int i = 0;
    // int arrayIndexes = 2;
    // char ** myArray = malloc(arrayIndexes * sizeof(*myArray));

    // //Allocate memory for each [x]
    // for (i = 0; i < arrayIndexes; i++)
    // {
    //     myArray[i] = malloc(254 * sizeof(char));
    //     sprintf(myArray[i], "string %d", i+1);
    // }

    // //Print out array values
    // printf("Before: \n");
    // for (i = 0; i < arrayIndexes; i++)
    //     printf("Array[%d]: %s\n", i, myArray[i]);

    // // TODO: Fix this to check the result before orphaning the old
    // //  value of myArray if an allocation failure ensues.
    // myArray = realloc(myArray, (arrayIndexes+1) * sizeof(*myArray));
    // ++arrayIndexes;

    // //Allocate memory for the new string item in the array
    // myArray[arrayIndexes-1] = malloc(254 * sizeof(char*));

    // //Populate a new value in the array
    // strcpy(myArray[arrayIndexes-1], "string 3"); //

    // //Print out array values
    // printf("After: \n");
    // for (i = 0; i < arrayIndexes; i++)
    //     printf("Array[%d]: %s\n",i, myArray[i]);

    // // TODO: write proper cleanup code just for good habits.
    return 0;
}
