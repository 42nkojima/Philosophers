/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 21:43:33 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/26 04:10:49 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static bool	philo_is_dead_locked(t_table *table, t_philo *philo)
{
	uint64_t	elapsed;

	elapsed = time_now_ms() - philo->last_meal_ms;
	return (elapsed >= (uint64_t)table->cfg.time_to_die);
}

static bool	try_report_death(t_table *table, t_philo *philo)
{
	int			philo_id;

	pthread_mutex_lock(&table->state_mutex);
	if (table->finished || table->death_printed)
	{
		pthread_mutex_unlock(&table->state_mutex);
		return (false);
	}
	if (!philo_is_dead_locked(table, philo))
	{
		pthread_mutex_unlock(&table->state_mutex);
		return (false);
	}
	philo_id = philo->index + 1;
	print_death_locked(table, philo_id);
	pthread_mutex_unlock(&table->state_mutex);
	return (true);
}

static bool	try_finish_all_fed(t_table *table)
{
	int	must;
	int	i;

	must = table->cfg.number_of_times_each_philosopher_must_eat;
	if (must < 0)
		return (false);
	pthread_mutex_lock(&table->state_mutex);
	if (table->finished)
	{
		pthread_mutex_unlock(&table->state_mutex);
		return (false);
	}
	i = 0;
	while (i < table->cfg.number_of_philosophers)
	{
		if (table->philos[i].eat_count < must)
		{
			pthread_mutex_unlock(&table->state_mutex);
			return (false);
		}
		i++;
	}
	table_finish_locked(table);
	pthread_mutex_unlock(&table->state_mutex);
	return (true);
}

void	*monitor_routine(void *arg)
{
	t_table	*table;
	int		count;
	int		i;

	table = arg;
	while (!table_is_finished(table))
	{
		i = 0;
		count = table->cfg.number_of_philosophers;
		while (i < count)
		{
			if (try_report_death(table, &table->philos[i]))
				return (NULL);
			i++;
		}
		if (try_finish_all_fed(table))
			return (NULL);
		time_sleep_ms(table, 1);
	}
	return (NULL);
}
