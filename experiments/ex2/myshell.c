#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define MAX_LINE 40

typedef struct {
	int error;
	char* past[MAX_LINE/2+1];
	int behind;
}hisCmd;


char input[MAX_LINE];
int background;             
char *args[MAX_LINE/2+1];
hisCmd history[MAX_LINE];
int last = 0;
int catch = 0;

/**
 * [handleSIGINT description]
 *
 * to handle the SIGINT
 * 
 * @Author   NinoLau
 */
void handleSIGINT(){
	char line[] = "\nHISTORY:";
	char sym[] = "\n";
	char space[] = " ";
	char background[] = "&";
	for(int i = last - 10 < 0 ? 0 : last - 10; i < last; i++){
		write(STDOUT_FILENO, line, strlen(line));
		write(STDOUT_FILENO, space, strlen(space));
		for(int j = 0; j < history[i].error; j++){
			write(STDOUT_FILENO, history[i].past[j], strlen(history[i].past[j]));
			write(STDOUT_FILENO, space, strlen(space));
		}
		if(history[i].behind)
			write(STDOUT_FILENO, background, strlen(background));
		
	}
	write(STDOUT_FILENO, sym, strlen(sym));
	catch = 1;
}

/**
 * [setup description]
 * @Author   NinoLau
 * setup() 
 * 
 * read the next command and eliminate the space writing to args[]
 * use NULL to represent the end
 * 
 * @param    input                    the input command
 * @param    args                     the args
 * @param    background               background
 * @return                            the pos of second argument
 */
int setup(char input[], char *args[],int *background)
{
    int length,   // the number of characters of the command
        start = -1,    // the start position of the command
        ct = 0;       // the next argument postion of the command

    // output the buffer content
    fflush(stdout); 

    // read the input and store into the input array
    length = read(STDIN_FILENO, input, MAX_LINE); 

    // if the command is ctrl + d, exit
    if (length == 0) {
    	exit(0);    
    }

    // if the command is wrong, exit
    if (length < 0) {
    	exit(-1);
    }

    // inspect each character in the buffer
    for (int i = 0; i < length; i++) { 
        switch (input[i]) {
	    case ' ':
	    // if the character is '\t' to sperate the characters
	    case '\t' :             
			if(start != -1){
                args[ct] = &input[start];    
		    	ct++;
			}
            input[i] = '\0'; 
			start = -1;
			break;
		// end of command
        case '\n':              
			if (start != -1){
                args[ct] = &input[start];     
		    	ct++;
			}
            input[i] = '\0';
            args[ct] = NULL; 
			break;
		// normal character
	    default : 
	    	// normal character to store           
			if (start == -1) {
		    	start = i;
			}
			// if character is '&', backgroud true 
            if (input[i] == '&'){  
		    	*background  = 1;         
                input[i] = '\0';
			}
		} 
     }    
     args[ct] = NULL; 
     return ct;
}


int main(void)
{
	signal(SIGINT, handleSIGINT);
	int re = 0;
	
    while(1){           
		background = 0;
 		printf("COMMAND: "); 
                                         
        re = setup(input,args,&background);       
        if(re != 0 && strlen(args[0]) == 1 && args[0][0] == 'r'){
        	if(last> 0 && re == 1){
				history[last].error = history[last - 1].error;
			    history[last].behind = history[last - 1].behind;
			    for(int k = 0; k < history[last].error; k++){
			    	history[last].past[k] = (char *)calloc(strlen(history[last - 1].past[k]), sizeof(char));
			    	strcpy(history[last].past[k], history[last - 1].past[k]);
			    }
			    last++;
        	}
        	else if(last> 0&& re == 2 && strlen(args[1]) == 1){
        		for(int i = last - 1; i >= 0; i--){
        			if(history[i].past[0][0] == args[1][0]){
        				history[last].error = history[i].error;
			        	history[last].behind = history[i].behind;
			        	for(int k = 0; k < history[last].error; k++){
			        		history[last].past[k] = (char *)calloc(strlen(history[i].past[k]), sizeof(char));
			        		strcpy(history[last].past[k], history[i].past[k]);
			        	}
			        	last++;
			        	break;
        			}
        		}
        	}
        	re = -1;
        }

        if(re > 0){
        	history[last].error = re;
        	history[last].behind = background;

        	for(int k = 0; k < re; k++){
        		history[last].past[k] = (char *)calloc(strlen(args[k]), sizeof(char));
        		strcpy(history[last].past[k], args[k]);
        	}
        	last++;
       	}

 		pid_t pid = fork();
 		if(pid == 0){
 			if(re == 0)
 				execvp(args[0], args);
 			else if(last > 0 && catch == 0){
 				execvp(history[last - 1].past[0], history[last - 1].past);
 			}
 			exit(0);
 		}
 		else if(pid > 0){
 			if(background == 0)
 				wait(0);
 		}
 		else{
 			perror("ERROR");
 		}

 		catch = 0;
    }
}