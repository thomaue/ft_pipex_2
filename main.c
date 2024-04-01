/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tauer <tauer@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 23:05:38 by tauer             #+#    #+#             */
/*   Updated: 2024/03/31 17:45:41 by tauer            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./ft_pipex.h"

void	free_tab(char **tab)
{
	int	i;

	i = 0;
	while (tab[i])
		free(tab[i++]);
	free(tab);
}

bool	setup(int argc, char **argv, char **envp, t_data *data)
{
	null_data(data);
	if (set_env(argc, argv, envp, data))
		return (terror("set_env failure"), true);
	if (set_arg(data))
		return (terror("set arg failure"), true);
	return (false);
}

bool	texec(t_data data, t_arg arg)
{
	if (execve(arg.path, arg.name, data.env.envp) == -1)
		return (true);
	return (false);
}

bool no_bonus(t_data *data, t_arg *arg) {
    int tube[2];
    int pid = 0;

    // Création du tube
    if (pipe(tube) == -1)
        return true;

    // Création d'un processus enfant
    pid = fork();
    if (pid == -1)
        return true;
    else if (pid != 0) {
        // Processus parent
        close(tube[0]);
        dup2(tube[1], STDOUT_FILENO);
        close(tube[1]);
        dup2(data->env.in_fd, STDIN_FILENO);
        close(data->env.in_fd);
        texec(*data, *arg);
    } else {
        // Processus enfant
        close(tube[1]);
        dup2(tube[0], STDIN_FILENO);
        close(tube[0]);
        int tube2[2];
        if (pipe(tube2) == -1)
            return true;
        int pid2 = fork();
        if (pid2 == -1)
            return true;
        else if (pid2 != 0) {
            // Processus parent
            close(tube2[0]);
            dup2(data->env.ou_fd, STDOUT_FILENO);
            close(data->env.ou_fd);
            texec(*data, *arg->next);
        } else {
            close(tube2[1]);
            dup2(tube2[0], STDIN_FILENO);
            close(tube2[0]);
            texec(*data, *arg);
        }
    }
    return false;
}


bool	pipex(t_data *data)
{
	t_arg *current;
	
	current = data->arg;
	if (data->env.argc == 4)
	{
		while(current)
		{
			if (current->pos == 1 || current->pos == 2)
				no_bonus(data, current);
			current = current->next;
		}
	}
	return (false);	
}

int	main(int argc, char **argv, char **envp)
{
	t_data	data;

	printf("\033c");
	if (setup(argc, argv, envp, &data))
		return (1);
	print_data(data);
	if (pipex(&data))
		return (2);
	return (free_list(&data), free_tab(data.env.path), 0);
}
