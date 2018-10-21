#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define INPUT_MAX 300
#define FILE_NAME "c.c"
#define COPY_NAME "copy.c"
#define COMPILE_COMMAND "gcc c.c"
// http://neugierig.org/software/c-repl/

FILE* fp;
int loop_function_flag = 0;

void init() {
	char str[INPUT_MAX] = "#include <stdio.h>\nint main(){\n";
	
	fp = fopen(FILE_NAME, "w");
	fputs(str, fp);
}
void copy(char* NAME, char* line, int flag) {
	FILE* cp;
	char buff[1000];

	if(!strcmp(NAME,COPY_NAME)) {
		fp = fopen(FILE_NAME, "r");
		cp = fopen(COPY_NAME, "w");
	} else {
		cp = fopen(FILE_NAME, "w");
		fp = fopen(COPY_NAME, "r");	
	}

	while(!feof(fp)) {
		fgets(buff, sizeof(buff), fp);
		if(!feof(fp)) {
			if(flag && strstr(buff, "(") && strstr(buff, ")") && !strstr(buff, "=") && !strstr(buff,"main()")) {
				if(strstr(buff, "(") && strstr(buff, ")") && !strstr(buff, "{")) {
					// do nothing
				} else 
					fputs(buff,cp);

				//if(loop_function_flag) 
				//	fputs(buff, cp);
			} else { 
				if(loop_function_flag && strstr(buff,"}"))  
					loop_function_flag = 0;
				fputs(buff, cp); 
			}
		}
	}
	fcloseall();
}
void insert(char* line) {
	if(fp == NULL) { 
		copy(FILE_NAME, line, 0);		
		fp = fopen(FILE_NAME, "a");
	}
	if(strstr(line, "(") && strstr(line, ")") && strstr(line, "{")) 
		loop_function_flag = 1;
	fputs(line, fp);
}
void terminate() {
	remove(FILE_NAME);
	remove(COPY_NAME);
	printf("exit!\n");
}
void compile(char* line) {
	fcloseall();
	copy(COPY_NAME, line, 1);
	
	// we wiil compile at all times if the syntax is not consist of loop or function
	if(strstr(line, "(") && strstr(line, ")") && strstr(line, "{")) { 
		// do nothing, just record into both file
	} else {
		fp = fopen(FILE_NAME, "a");
		fputc('}', fp);
		fclose(fp);
		if(!loop_function_flag) { 
			system(COMPILE_COMMAND);
			char buf[300];
			FILE* p = popen("./a.out", "r");
			while(fgets(buf, 300, p))
				printf("%s", buf);
		}
	}
	fp = NULL;
}

int main() {
	char line[INPUT_MAX];
	
	init();

	while(1) {
		printf("> ");
		memset(line, 0, sizeof(line));
		fgets(line, sizeof(line), stdin);
		if(!strncmp(line, "exit",4)) 
			break;
		insert(line);
		compile(line);
	}

	terminate();

	return 0;
}
