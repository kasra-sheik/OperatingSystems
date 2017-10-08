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
#include <stdbool.h>
#include <ctype.h>



//helper function to output all the results of word calculations 
void outputResults(char ** words, int * wordCount, int outputCount, int totalWords, int uniqueWords) {
	int i = 0;
	bool limitOutput = false; 

	printf("All done (successfully read %d words; %d unique words).\n", totalWords, uniqueWords);

	if(outputCount > 0 && outputCount < uniqueWords){
		limitOutput = true; 
		printf("First %d words (and corresponding counts) are:\n", outputCount);
	}
	else{
		printf("All words (and corresponding counts) are:\n");
	}

	while(i < uniqueWords) {
		if(limitOutput == true && i == outputCount){
			break;
		}
		 //printf("%d\n", i);
		 printf("%s -- ", words[i]);
		 printf("%d\n", wordCount[i]);
		 fflush(stdout);
		i += 1;
	}

}

//function to check whether our words array contains a specific word, used for handeling whether to add a new word or incremenet our wordcount array
int containsWord(char * word, char ** words, int uniqueWords) {
	int i = 0;
 	while(i < uniqueWords) {
 		if(strcmp(words[i], word) == 0) {
 			return 1;
 		}
 		i += 1;
 	}
 	return 0;
}
//grabs the index of the given word in the words array, we assume the function to return a non zero value because it is called after contains word
int indexOfWord(char * word, char ** words, int uniqueWords) {
	int i = 0; 
	while(i < uniqueWords) {
		if(strcmp(words[i], word) == 0){
			return i;
		}
		i += 1;
	}
	return 0;
}
//returns true if the string is truly alpha numeric DEPRECATED
int isAlphaNumericString(char * string) {
	for(int i = 0; i < strlen(string); i++){
		if(isalnum(string[i]) == 0){
			return 0;
		}
	}
	return 1;
}
//function to handle all the dynamic memory allocation and logic behind adding words..
void processFiles(char * directory, int outputCount) {
	//create directory and initialize pointers 
	DIR *directoryToSearch;
	struct dirent *file;
	directoryToSearch = opendir(directory);

	int arraySize = 16;
	char ** words = (char**) calloc(arraySize, sizeof(*words));
	int * wordCount = (int *) calloc(arraySize, sizeof(wordCount));
	int uniqueWords = 0;
	int totalWords = 0;

	printf("Allocated initial parallel arrays of size %d.\n", arraySize);

	if(directoryToSearch == NULL) {
		perror("ERROR: Did not find a directory to search.");
    	return;
	}	
	
	chdir(directory);
	//read in each file.. check if its a regular file 
	while((file = readdir(directoryToSearch)) != NULL) {
		struct stat sb;
		lstat(file->d_name, &sb);

		if(S_ISREG(sb.st_mode)) {

			//bit of code to completley remove alpha numeric characters from the given text file and replace them with 
			//spaces to avoid insufficient word counts because of non alpha chars
			FILE *fileToWrite;
		    int character;
		    fileToWrite = fopen(file->d_name, "r+");
		    while ((character = fgetc(fileToWrite)) != EOF)
		    {
		        if(isalnum(character) == 0 && character != ' ' && character!= '\n'){
		            fseek(fileToWrite, -1, SEEK_CUR);
		            fputc(' ',fileToWrite);
		            fseek(fileToWrite, 0, SEEK_CUR);
		        }
		    }
		    fclose(fileToWrite);



			FILE *selected_file = fopen(file->d_name, "r");
			char sWord[80];

		 	while(fscanf(selected_file, "%s", sWord) != EOF) {
		 		//when we find a valid word.. find the correct index and do all handeling..
		 		if(strlen(sWord) >= 2){
		 			int i = 0;
		 			while(i < uniqueWords) {
		 				i += 1;
		 			}
		 			//add it to words if it isn't already found
					if(containsWord(sWord, words, uniqueWords) == 0){
						words[i] = calloc(sizeof(sWord), strlen(sWord) + 1);
						strcpy(words[i], sWord);

						wordCount[i] = 1;
						uniqueWords += 1;
					}
					//increment index if it is
					else {
						int selectedIndex = indexOfWord(sWord,words, uniqueWords);
						int selectedCount = wordCount[selectedIndex];
						selectedCount += 1;
						wordCount[selectedIndex] = selectedCount;
					}

					//realloc parallel arrays if we are about to go over the bounds of the allocated space
					if(i + 1 == arraySize){
	 					arraySize += 16;
	 					words = realloc(words, arraySize * sizeof(*words));
	 					wordCount = realloc(wordCount, arraySize * sizeof(*wordCount));
	 					printf("Re-allocated parallel arrays to be size %d.\n", arraySize);
	 					fflush(stdout);
							
		 			}
					totalWords += 1;

		 		}
			}
		
			fclose(selected_file);		

		}
	}


	outputResults(words, wordCount, outputCount, totalWords, uniqueWords);


	//free pointers and close directory
	int i = 0;
	while(i < uniqueWords){
		free(words[i]);
		i+=1;
	}
	free(words);
	free(wordCount);
	closedir(directoryToSearch);




}

int main(int argc, char ** argv) {

	if(argc < 2) {
		perror("ERROR: Did not find a directory to search.");
    	return EXIT_FAILURE;
	}

	//handle our output count.. is initially set to zero which mimics null
	int outputCount = 0;
	if(argc > 2){
		//convert to int..
		outputCount = atoi(argv[2]);
	}
	
	processFiles(argv[1], outputCount);

	return 0;
	
}