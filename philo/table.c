/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 17:00:00 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/26 03:35:00 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdlib.h>

static int	table_forks_create(t_table *table, int count)
{
	int	i;

	table->forks = malloc(sizeof(*table->forks) * count);
	table->fork_reserved = calloc(count, sizeof(*table->fork_reserved));
	if (!table->forks || !table->fork_reserved)
		return (free(table->forks), free(table->fork_reserved), -1);
	i = 0;
	while (i < count)
	{
		if (pthread_mutex_init(&table->forks[i], NULL) != 0)
		{
			while (i > 0)
				pthread_mutex_destroy(&table->forks[--i]);
			free(table->forks);
			free(table->fork_reserved);
			table->forks = NULL;
			table->fork_reserved = NULL;
			return (-1);
		}
		i++;
	}
	return (0);
}

static int	table_philos_create(t_table *table, int count)
{
	int	i;

	table->philos = malloc(sizeof(*table->philos) * count);
	if (!table->philos)
		return (-1);
	i = 0;
	while (i < count)
	{
		table->philos[i].id = i;
		table->philos[i].table = table;
		table->philos[i].eat_count = 0;
		table->philos[i].left_fork_index = i;
		table->philos[i].right_fork_index = (i + 1) % count;
		table->philos[i].hungry = false;
		i++;
	}
	table->start_time_ms = time_now_ms();
	i = 0;
	while (i < count)
	{
		table->philos[i].last_meal_ms = table->start_time_ms;
		i++;
	}
	return (0);
}

bool	table_is_finished(t_table *table)
{
	bool	finished;

	pthread_mutex_lock(&table->state_mutex);
	finished = table->finished;
	pthread_mutex_unlock(&table->state_mutex);
	return (finished);
}

int	table_init(t_table *table, const t_config *cfg)
{
	int	count;

	table->cfg = *cfg;
	table->start_time_ms = 0;
	table->finished = false;
	table->death_printed = false;
	table->philos = NULL;
	table->forks = NULL;
	table->fork_reserved = NULL;
	if (pthread_mutex_init(&table->state_mutex, NULL) != 0)
		return (-1);
	if (pthread_mutex_init(&table->print_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&table->state_mutex);
		return (-1);
	}
	count = cfg->number_of_philosophers;
	if (table_philos_create(table, count) == -1
		|| table_forks_create(table, count) == -1)
	{
		table_destroy(table);
		return (-1);
	}
	return (0);
}

void	table_destroy(t_table *table)
{
	int	count;
	int	i;

	if (!table)
		return ;
	count = table->cfg.number_of_philosophers;
	if (table->forks)
	{
		i = 0;
		while (i < count)
		{
			pthread_mutex_destroy(&table->forks[i]);
			i++;
		}
		free(table->forks);
		table->forks = NULL;
	}
	free(table->fork_reserved);
	table->fork_reserved = NULL;
	free(table->philos);
	table->philos = NULL;
	pthread_mutex_destroy(&table->state_mutex);
	pthread_mutex_destroy(&table->print_mutex);
}
