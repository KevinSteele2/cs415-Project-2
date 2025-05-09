#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

#include "string_parser.h"

void script_print (pid_t* pid_ary, int size);

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

    pid_t *pid_ary = (pid_t *)malloc(sizeof(pid_t) * size);

    int i = 0;
    while(getline(&line, &len, file) != -1){
        command_line cmd = str_filler(line, " ");
        pid_ary[i] = fork();

        if(pid_ary[i] < 0){
            fprintf(stderr, "fork failed\n");
            free_command_line(&cmd);
            free(pid_ary);
            fclose(file);
            exit(-1);
        }
        else if(pid_ary[i] == 0){
            execvp(cmd.command_list[0], cmd.command_list);
            exit(0);
        }
        free_command_line(&cmd);
        i++;
    }      

	script_print(pid_ary, size);

    for(int p = 0; p < size; p++){
        waitpid(pid_ary[p], NULL, 0);
    }

	free(pid_ary);
    free(line);
    fclose(file);
    return 0;
}


void script_print (pid_t* pid_ary, int size)
{
	FILE* fout;
	fout = fopen ("top_script.sh", "w");
	fprintf(fout, "#!/bin/bash\ntop");
	for (int i = 0; i < size; i++)
	{
		fprintf(fout, " -p %d", (int)(pid_ary[i]));
	}
	fprintf(fout, "\n");
	fclose (fout);

	char* top_arg[] = {"gnome-terminal", "--", "bash", "top_script.sh", NULL};
	pid_t top_pid;

	top_pid = fork();
	{
		if (top_pid == 0)
		{
			if(execvp(top_arg[0], top_arg) == -1)
			{
				perror ("top command: ");
			}
			exit(0);
		}
	}
}


