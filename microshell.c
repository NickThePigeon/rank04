#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void	ft_putstr_err(char *str)
{
	int i;

	i = 0;
	while (str[i])
		i++;
	write(2, str, i);
}

int ft_execute(char **argv, char **envp, int i)
{
	argv[i] = NULL;
	execve(argv[0], argv, envp);
	ft_putstr_err("error: cannot execute ");
	ft_putstr_err(argv[0]);
	ft_putstr_err("\n");
	return (1);
}

int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	int i, tmpfd, pid;
	int fd[2];

	i = 0;
	pid = 0;
	tmpfd = dup(STDIN_FILENO);
	while (argv[i] && argv[i + 1])
	{
		argv = &argv[i + 1];
		i = 0;
		while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
			i++;

		if (strcmp(argv[0], "cd") == 0)
		{
			if (i != 2)
				ft_putstr_err("error: cd: bad arguments");
			if (chdir(argv[1]) < 0)
			{
				ft_putstr_err("error: cd: cannot change directory to ");
				ft_putstr_err(argv[1]);
				ft_putstr_err("\n");
			}
		}
		else if (i != 0 && (argv[i] == NULL || (strcmp(argv[i], ";") == 0)))
		{
			pid = fork();
			if (pid == 0)
			{
				dup2(tmpfd, STDIN_FILENO);
				close(tmpfd);
				if (ft_execute(argv, envp, i))
					return (1);
			}
			else
			{
				close(tmpfd);
				while (waitpid(-1, NULL, WUNTRACED) != -1) ;
				tmpfd = dup(STDIN_FILENO);
			}
		}
		else if (i != 0 && strcmp(argv[i], "|") == 0)
		{
			pipe(fd);
			pid = fork();
			if (pid == 0)
			{
				dup2(tmpfd, STDIN_FILENO);
				dup2(fd[1], STDOUT_FILENO);
				close(fd[0]);
				close(fd[1]);
				close(tmpfd);
				if (ft_execute(argv, envp, i))
					return (1);
			}
			else
			{
				close(tmpfd);
				close(fd[1]);
				tmpfd = fd[0];
			}
		}
	}
	close(tmpfd);
	return (0);
}