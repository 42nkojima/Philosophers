/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 05:48:09 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/26 03:39:28 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	start_philosophers(t_table *table, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		if (pthread_create(&table->philos[i].thread, NULL, philo_routine,
				&table->philos[i]) != 0)
			return (i);
		i++;
	}
	return (count);
}

static void	join_philosophers(t_table *table, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(table->philos[i].thread, NULL);
		i++;
	}
}

static void	stop_simulation(t_table *table)
{
	pthread_mutex_lock(&table->state_mutex);
	table->finished = true;
	pthread_mutex_unlock(&table->state_mutex);
}

static int	run_simulation(t_table *table, pthread_t *mon)
{
	int	count;
	int	started;

	count = table->cfg.number_of_philosophers;
	if (pthread_create(mon, NULL, monitor_routine, table) != 0)
		return (-1);
	started = start_philosophers(table, count);
	if (started != count)
	{
		stop_simulation(table);
		join_philosophers(table, started);
		pthread_join(*mon, NULL);
		return (-1);
	}
	pthread_join(*mon, NULL);
	join_philosophers(table, count);
	return (0);
}

int	main(int ac, char **av)
{
	t_config	cfg;
	t_table		table;
	pthread_t	mon;

	if (parse_config(ac, av, &cfg) == -1)
		return (1);
	if (table_init(&table, &cfg) == -1)
		return (1);
	if (run_simulation(&table, &mon) == -1)
		return (table_destroy(&table), 1);
	table_destroy(&table);
	return (0);
}
