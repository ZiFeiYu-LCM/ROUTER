#include "test.h"
void* connectplc(){

}

void* lcmfun(void* data){
	/*pid_t pid;
	int status;
	pid = fork();
	if(pid==-1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if(pid==0){
		connectplc();
		exit(EXIT_SUCCESS);
	}else if(pid>0){
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
		    printf("Child process exited with status %d\n", WEXITSTATUS(status));
		} else {
		    printf("Child process terminated abnormally\n");
		}
		printf("parent process is end\n");
	} else {
		perror("Fork failed");
		exit(1);
	}*/
	
}
