#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
int main() {
	/* Max command length is 80.  Need one more for the null. */
	char command[81];
	char *args[20];
	char *p;
	int should_run,mustWait,count;
	pid_t pid;
	pid_t children[40];
	
	should_run = 1;
	for(count = 0;count > 40;count++)
		children[count] = 0;

	while(should_run == 1) {
		/* reset wait variable */
		mustWait = 0;
		
		/* Output a prompt */
		printf("osh> ");
		fflush(stdout);
		
		/* Input the command */
		fgets(command,81,stdin);
		
		//check the background process array to see if any
		//children processes need to be waited on
		for(count = 0;count < 40;count++){
			if(children[count] != 0){
				if(waitpid(children[count],NULL,WNOHANG) != 0){
					children[count] = 0;
				}
			}
		}
		
		//strip command of \n
		if (command[strlen(command) - 1] == '\n')
			command[strlen(command) - 1] = '\0';
	
		/* strtok will return a string that includes everything up to the
		first delimiter So each time through the loop, we get one argument in p */
		p = strtok(command, " ");

		/* check for exit */
		if(strcmp(p,"exit") == 0)
			break;
		
		args[0] = p;
		count = 1;
		while(p) {
			/* Now get the next argument */
			p = strtok(NULL," ");

			/* check for the & */
			if(p != NULL){
				if(strcmp(p,"&") == 0){
					mustWait = 1;
					args[count] = NULL;
					break;
				}
			}
			args[count] = p;
			count++;
		}
		/* create child process */
		pid = fork();
		
		/* child process */
		if (pid == 0) {
			if((execvp(args[0], args)) == -1)
				printf("Command not found\n");
		}
		/* If &, parent process waits */
		else if(mustWait == 0){
			wait(NULL);
		}
		//add the child to the background process array
		else{
			for(count = 0;count < 40;count++){
				if(children[count] == 0){
					children[count] = pid;
					break;
				}
			}
		}
	}
	return 0;
}