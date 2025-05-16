#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

#include "string_parser.h"

int main(int argc,char*argv[])
{
	if (argc != 2)
	{
		printf ("Wrong number of arguments\n");
		exit (0);
	}

    char *line = NULL;
    size_t len = 0;
    FILE *file = fopen(argv[1], "r");
    if(!file){
        fprintf(stdout, "Error with input file\n");
        return EXIT_FAILURE;
    }

    int size = 0;
    while (getline(&line, &len, file) != -1){
        size++;
    }
    rewind(file);

    pid_t *pid_array = (pid_t *)malloc(sizeof(pid_t) * size);

    int i = 0;
    while(getline(&line, &len, file) != -1){
        command_line cmd = str_filler(line, " ");
        pid_array[i] = fork();


        if(pid_array[i] < 0){
            fprintf(stderr, "fork failed\n");
            free_command_line(&cmd);
            free(pid_array);
            fclose(file);
            exit(-1);
        }
        else if(pid_array[i] == 0){
            //Wait for SIGUSR1 signal
            sigset_t set;
            sigemptyset(&set);
            sigaddset(&set, SIGUSR1);
            sigprocmask(SIG_BLOCK, &set, NULL);
            int sig;
            sigwait(&set, &sig);

            execvp(cmd.command_list[0], cmd.command_list);
            exit(0);
        }
        free_command_line(&cmd);
        i++;
    }
    for (int j = 0; j<size; j++){
        kill(pid_array[j], SIGUSR1);
    } 
    for(int j = 0; j<size; j++){
        kill(pid_array[j], SIGSTOP);
        printf("SIGSTOP sending to %d\n", pid_array[j]);
    }
    for(int j=0; j<size; j++){
        kill(pid_array[j], SIGCONT);
        printf("SIGCONT sending to %d\n", pid_array[j]);
    }

    for(int j= 0; j < size; j++){
        waitpid(pid_array[j], NULL, 0);
    }

	free(pid_array);
    free(line);
    fclose(file);
    return 0;
}


