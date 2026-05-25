/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 21:43:33 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/26 02:48:57 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
monitor_routine:
- table を引数に取る(void * -> t_table *)
- finished になるまで巡回
- 各 philo: state_mutex 下で餓死判定と print_death_locked を一括
- ループ末尾: 短いusleep (負荷と 10ms 要件のバランス)
*/

static bool	table_finished(t_table *table)
{
	bool	finished;

	pthread_mutex_lock(&table->state_mutex);
	finished = table->finished;
	pthread_mutex_unlock(&table->state_mutex);
	return (finished);
}

static bool	try_report_death(t_table *table, t_philo *philo)
{
	uint64_t	elapsed;
	int			philo_id;

	pthread_mutex_lock(&table->state_mutex);
	if (table->finished || table->death_printed)
	{
		pthread_mutex_unlock(&table->state_mutex);
		return (false);
	}
	elapsed = time_now_ms() - philo->last_meal_ms;
	if (elapsed < (uint64_t)table->cfg.time_to_die)
	{
		pthread_mutex_unlock(&table->state_mutex);
		return (false);
	}
	philo_id = philo->id + 1;
	print_death_locked(table, philo_id);
	pthread_mutex_unlock(&table->state_mutex);
	return (true);
}

void	*monitor_routine(void *arg)
{
	t_table	*table;
	int		count;
	int		i;

	table = arg;
	while (!table_finished(table))
	{
		i = 0;
		count = table->cfg.number_of_philosophers;
		while (i < count)
		{
			if (try_report_death(table, &table->philos[i]))
				return (NULL);
			i++;
		}
		time_sleep_ms(table, 1);
	}
	return (NULL);
}
