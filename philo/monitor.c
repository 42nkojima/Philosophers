/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 21:43:33 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/25 22:08:53 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdint.h>
#include <string.h>
#include <unistd.h>

/*
monitor_routine:
- table を引数に取る(void * -> t_table *)
- finished になるまで巡回
- 各 philo: (now - last_meal_ms) >= time_to_die なら print_death
- last_meal / finished は state_mutex で読む (書くのは print_death 側も)
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

static bool	philo_is_starved(t_table *table, t_philo *philo)
{
	uint64_t	last_meal;
	uint64_t	elapsed;

	pthread_mutex_lock(&table->state_mutex);
	last_meal = philo->last_meal_ms;
	pthread_mutex_unlock(&table->state_mutex);
	elapsed = time_now_ms() - last_meal;
	return (elapsed >= (uint64_t)table->cfg.time_to_die);
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
			if (philo_is_starved(table, &table->philos[i]))
			{
				print_death(table, table->philos[i].id + 1);
				return (NULL);
			}
			i++;
		}
		time_sleep_ms(table, 1);
	}
	return (NULL);
}
