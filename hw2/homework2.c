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
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>




//given a operand and an operation.. calculate value of the expression..
int calculate(int * operands, int operandCount, char operation) {
	int calculation = 0;
	int indexToStart = 0;


	if(operation == '*'){
		calculation = 1;
	}
	else if(operation == '-'){
		indexToStart = 1;
		calculation = operands[0];
	}
	else if(operation == '/'){
		indexToStart = 1;
		calculation = operands[0];
	}


	for(int i = indexToStart; i < operandCount; i++){
		if(operation == '*') {
			calculation *= operands[i];
		}
		else if(operation == '-') {
			calculation -= operands[i];
		}
		else if(operation == '+'){
			calculation += operands[i];
		}
		else if(operation == '/'){
			calculation /= operands[i];
		}
	}

	return calculation;
}
//given an i value for a digit will return true if the desired number is a negative number..
bool isNegative(int i, char * expr) {
	//assuming i is the first index of the digit..
	while(expr[i] != ' '){
		if(expr[i] == '-'){
			return true;
		}
		i -= 1;
	}
	return false;
}

char* getSubExpression(int i, char * expr) {
	char subExpression[128];
	int subExpressionCount = 0;
	while(expr[i] != ')'){
		subExpression[subExpressionCount] = expr[i];
		subExpressionCount += 1;
		i += 1;
	}
	subExpression[subExpressionCount] = ')';
	char * subExp;
	subExp = malloc(strlen(subExpression) + 1 * sizeof(subExpression));
	subExp = subExpression;
	return subExp;
}

void process_expr(char * expr, int i, int parent_pid, int writePipe) {
	printf("PID %d: My expression is \"%s\"\n", getpid(), expr);

	char currentOperator;

	int * digits; 
	digits = malloc(16 * sizeof(digits));
	int digitCount = 0;

	int * childProcesses;
	int childProcessesCount = 0;
	childProcesses = malloc(32 * sizeof(childProcesses));
	int p[2];


	//initialize file descriptor..once 

	while(i < strlen(expr)){
		if(childProcessesCount > 0){
			//wait for children operands process them and add them to digits before reading anything else in 
			if(childProcessesCount > 0){
				for(int j = 0; j <= childProcessesCount; j++){
					int child_pid = childProcesses[j];
					int status;
					waitpid(child_pid, &status, 0);
					//printf("we are here..waiting for pid: %d\n", child_pid);

					//read from this child..

					char child_buffer[80];
				    int bytes_read = read(p[0],child_buffer,32);
				    child_buffer[bytes_read] = '\0';
				   	int currentOperand = atoi(child_buffer);

				   	//printf("currentOperand read: %d\n",currentOperand);
				   	childProcesses[j] = 0;
				   	childProcessesCount -= 1;

				   	digits[digitCount] = currentOperand;
				   	digitCount += 1;


				}
			}
		}

		if(expr[i] == '('){
			if(i == 0){
				currentOperator = expr[i + 1];
				printf("PID %d: Starting \"%c\" operation\n", getpid(), currentOperator);
				i += 1;
				continue;
			}
			else {
				char subExpression [128];
				strcpy(subExpression,getSubExpression(i, expr));
				

				pipe( p );
				pid_t pid;
				pid = fork();

				if(pid == 0){
					process_expr(subExpression,0, getpid(), p[1]);
					return;
				}
				else{
					//add child process too
					int status;
					waitpid(pid, &status, 0);
					childProcesses[childProcessesCount] = pid;
					childProcessesCount += 1;
				
					i += strlen(subExpression);

					continue;
				}
			}
		}
		//we know we've reached an operand..
		else if(isdigit(expr[i])) {
			//check for negative here..

			char currentNumString[32] = "";
			int operandDigits = 0;
			if(isNegative(i, expr) == true){
				operandDigits += 1;
				currentNumString[0] = '-';
			}
			while(isdigit(expr[i])) {
				currentNumString[operandDigits] = expr[i];
				operandDigits += 1;
				i += 1;
			}

			pipe( p );
			pid_t pid; 
			pid = fork();


			if(pid == 0) {
				printf("PID %d: My expression is \"%s\"\n", getpid(), currentNumString);

				close( p[0] );   
    			p[0] = -1;

			    write( p[1], currentNumString, strlen(currentNumString) + 1);
			   	printf("PID %d: Sending \"%s\" on pipe to parent\n", getpid(), currentNumString);

				return;
			}
			else {

				int status;
				waitpid(pid, &status, 0);
			    	
			    close( p[1] );    //close the write end of the pipe 
   				p[1] = -1;

				char buffer[80];
			    int bytes_read = read(p[0],buffer,32);
			    buffer[bytes_read] = '\0';
			   	int currentOperand = atoi(buffer);

			   	digits[digitCount] = currentOperand;
				digitCount += 1;
			

			    
			}			
		}

		i += 1;
	}

	//child sending entire operand calculation up to parent.
	if(parent_pid > 0){
		if(childProcessesCount > 0){
     		for(int i = 0; i < childProcessesCount; i++) {
     			int pid = childProcesses[i];
     			int status;
     			waitpid(pid, &status, 0 );
     			//wait(NULL!);

     			close( p[1] );  
    			p[1] = -1;

				//read from the pipe..
				char buffer[80];
	    		int bytes_read = read( p[0], buffer, 32);
	   			buffer[bytes_read] = '\0';

				int childCalculation = atoi(buffer);
				digits[digitCount] = childCalculation;
				digitCount += 1;
     		}
     	}

     	int childOperandCalculation = calculate(digits, digitCount, currentOperator);
		char childCalculationString[32];
		sprintf(childCalculationString, "%d", childOperandCalculation);

		close( p[0] );   
     	p[0] = -1;
     	pipe(p); 

	    write(writePipe, childCalculationString, strlen(childCalculationString) + 1);

		printf("PID %d: Processed \"%s\"; sending \"%s\" on pipe to parent\n", getpid(), expr, childCalculationString);
		
		return;

	}
	if(parent_pid == 0){
		for(int i = 0; i < childProcessesCount; i++){

			// printf("Waiting for pid: %d\n",childProcesses[i]);
			int pid = childProcesses[i];
			int status;
			waitpid(pid, &status, 0);

			close( p[1] );  
    		p[1] = -1;

			//read from the pipe..
			char buffer[80];
    		int bytes_read = read( p[0], buffer, 32);
   			buffer[bytes_read] = '\0';


			int childCalculation = atoi(buffer);
			digits[digitCount] = childCalculation;
			digitCount += 1;

			//printf("I am the parent and I read %d bytes from pid %d\n", bytes_read, pid);


		}

		// printf("printing parents digits...\n");
		// for(int i = 0; i < digitCount; i++){
		// 	printf("digit[%d]: %d\n",i, digits[i]);
		// }


		printf("PID %d: Processed \"%s\"; final answer is \"%d\"\n", getpid(), expr, calculate(digits, digitCount, currentOperator));

	}

	return;

}

int main(int argc, char ** argv) {

	if(argc < 2){
		perror("ERROR: Invalid arguments\nUSAGE: ./a.out <input-file>");
    	return EXIT_FAILURE;
	}


	char * fileName = argv[1];
	char expression [128];
	int character; 
	FILE *inputFile; 
	inputFile = fopen(fileName, "r");

	int i = 0;
	while((character = fgetc(inputFile)) != EOF) {
		if(character == '\n'){
			break;
		}
		expression[i] = character;
		i += 1;
	} 



	process_expr(expression, 0, 0, 0);



	return 0;
}
