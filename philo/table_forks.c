/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table_forks.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 08:00:00 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/30 08:00:00 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdlib.h>
#include <string.h>

static void	table_clear_fork_storage(t_table *table)
{
	free(table->forks);
	free(table->fork_reserved);
	table->forks = NULL;
	table->fork_reserved = NULL;
}

static void	table_destroy_fork_mutexes(t_table *table, int initialized_count)
{
	int	i;

	i = 0;
	while (i < initialized_count)
	{
		pthread_mutex_destroy(&table->forks[i]);
		i++;
	}
}

int	table_forks_create(t_table *table, int count)
{
	int	i;

	table->forks = malloc(sizeof(*table->forks) * count);
	table->fork_reserved = malloc(sizeof(*table->fork_reserved) * count);
	if (!table->forks || !table->fork_reserved)
		return (table_clear_fork_storage(table), -1);
	memset(table->fork_reserved, 0, sizeof(*table->fork_reserved) * count);
	i = 0;
	while (i < count)
	{
		if (pthread_mutex_init(&table->forks[i], NULL) != 0)
		{
			table_destroy_fork_mutexes(table, i);
			return (table_clear_fork_storage(table), -1);
		}
		i++;
	}
	return (0);
}

void	table_forks_destroy(t_table *table, int count)
{
	if (!table->forks)
		return ;
	table_destroy_fork_mutexes(table, count);
	table_clear_fork_storage(table);
}
