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

static int	table_philos_create(t_table *table, int count)
{
	int	i;

	table->philos = malloc(sizeof(*table->philos) * count);
	if (!table->philos)
		return (-1);
	table->start_time_ms = time_now_ms();
	i = 0;
	while (i < count)
	{
		table->philos[i].index = i;
		table->philos[i].table = table;
		table->philos[i].last_meal_ms = table->start_time_ms;
		table->philos[i].eat_count = 0;
		table->philos[i].left_fork_index = i;
		table->philos[i].right_fork_index = (i + 1) % count;
		table->philos[i].wants_to_eat = false;
		i++;
	}
	return (0);
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

	if (!table)
		return ;
	count = table->cfg.number_of_philosophers;
	table_forks_destroy(table, count);
	free(table->philos);
	table->philos = NULL;
	pthread_mutex_destroy(&table->state_mutex);
	pthread_mutex_destroy(&table->print_mutex);
}
