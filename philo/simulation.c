/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 08:00:00 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/30 10:14:15 by nkojima          ###   ########.fr       */
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

static int	start_monitor(pthread_t *monitor, t_table *table)
{
	if (pthread_create(monitor, NULL, monitor_routine, table) != 0)
		return (-1);
	return (0);
}

int	simulation_run(t_table *table)
{
	pthread_t	monitor;
	int			count;
	int			started;

	count = table->cfg.number_of_philosophers;
	if (start_monitor(&monitor, table) == -1)
		return (-1);
	started = start_philosophers(table, count);
	if (started < count)
	{
		table_finish(table);
		pthread_join(monitor, NULL);
		join_philosophers(table, started);
		return (-1);
	}
	pthread_join(monitor, NULL);
	join_philosophers(table, count);
	return (0);
}
