#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if (argc = !2)
	{
		fprintf(stderr, "You have to give number of proccesses N!");
		exit(EXIT_FAILURE);
	}

	int N = atoi(argv[1]);
	if (N <= 0)
	{
		fprintf(stderr, "N has to be positive integer!");
		exit(EXIT_FAILURE);
	}

	pid_t *children_arr = (pid_t *)malloc(N * sizeof(pid_t));
	if (children_arr == NULL)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < N; i++)
	{
		pid_t pid = fork();
		if (pid < 0)
		{ // Case of error
			perror("fork");
			exit(EXIT_FAILURE);
		}
		else if (pid == 0)
		{ // Case of child proccess
			pid_t child_pid = getpid();
			pid_t parent_pid = getppid();
			printf("Child proccess created with PID: %d\n", child_pid);

			char file[256];
			snprintf(file, sizeof(file), "output_%d.txt", child_pid);
			int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd < 0)
			{
				perror("open file");
				exit(EXIT_FAILURE);
			}

			char text[256];
			snprintf(text, sizeof(text), "Child's PID: %d, Parent PID: %d\n", child_pid, parent_pid);
			if (write(fd, text, strlen(text)) < 0)
			{
				perror("write file");
				close(fd);
				exit(EXIT_FAILURE);
			}

			close(fd);
			exit(EXIT_SUCCESS);
		}
		else
		{ // Case of parent proccess
			children_arr[i] = pid;
		}
	}

	for (int i = 0; i < N; i++)
	{ // I know that child proccesses will take longer
		wait(NULL);
	}

	printf("\nParent reading file contents:\n");
	for (int i = 0; i < N; i++)
	{
		char file[256];
		snprintf(file, sizeof(file), "output_%d.txt", children_arr[i]);
		int fd = open(file, O_RDONLY);
		if (fd < 0)
		{
			perror("open file");
			exit(EXIT_FAILURE);
		}

		char text[256];
		if (read(fd, text, sizeof(text) - 1) < 0)
		{
			perror("read file");
			close(fd);
			continue;
		}

		printf("%s\n", text);
		close(fd);
	}

	free(children_arr);

	return 0;
}
