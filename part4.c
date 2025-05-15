#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

#include "string_parser.h"

int process = 0;
int *isrunning;
pid_t *pid_array;
int size;

void get_proc(pid_t pid) {
    // reading from stat
    char path[256];
    sprintf(path, "/proc/%d/stat", pid);
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Error opening /proc stat");
        return;
    }

    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), file)) {
        long utime, stime;
        sscanf(buffer, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %ld %ld", &utime, &stime);
        printf("PID %d\n User time: %ld\n System time: %ld\n", pid, utime, stime);
    }
    fclose(file);

    //reading from statm
    sprintf(path, "/proc/%d/statm", pid);
    file = fopen(path, "r");
    if(!file){
        perror("Error /proc statm");
        return;
    }
    long vmsize, rss;
    if(fscanf(file, "%ld %ld", &vmsize, &rss) == 2){
        printf("Virtual Memory Size: %ld pages\n", vmsize);
        printf("Resident Set Size: %ld pages\n", rss);
    }
    fclose(file);
}

void alarm_handling(int sig){
    if(isrunning[process]){
        kill(pid_array[process], SIGSTOP);
        printf("SIGSTOP sending to %d\n", pid_array[process]);
    }
    while(!isrunning[process]){
        process = (process+1) % size;
    }

    kill(pid_array[process], SIGCONT);
    printf("SIGCONT sending to %d\n", pid_array[process]);

    printf("Data from proc:\n")
    for(int j = 0; j < size; j++){
        if(isrunning[j]){
            get_proc(pid_array[i]);
        }
    }
    alarm(1);
}

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

    size =0;
    while (getline(&line, &len, file) != -1){
        size++;
    }
    rewind(file);

    pid_array = (pid_t *)malloc(sizeof(pid_t) * size);
    isrunning=(int*)malloc(sizeof(int) * size);

    int i = 0;
    while(getline(&line, &len, file) != -1){
        command_line cmd = str_filler(line, " ");
        pid_array[i] = fork();


        if(pid_array[i] < 0){
            fprintf(stderr, "fork failed\n");
            free_command_line(&cmd);
            free(pid_array);
            free(isrunning);
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
        isrunning[i] = 1;
        i++;
    }
    for (int j = 0; j<size; j++){
        kill(pid_array[j], SIGUSR1);
        printf("SIGUSR1 sending to %d\n", pid_array[j]);
    } 

    kill(pid_array[0], SIGCONT);
    printf("%d starting\n", pid_array[0]);
    signal(SIGALRM, alarm_handling);
    alarm(1);

    int notrunning = 0;
    while(notrunning < size){
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        if(pid > 0){
            for (int i = 0; i < size; i++){
                if(pid_array[i] == pid){
                    isrunning[i] = 0;
                    notrunning++;
                    printf("%d terminated\n", pid);
                    break;
                }
            }
        }
    }

	free(pid_array);
    free(line);
    free(isrunning);
    fclose(file);
    return 0;
}
