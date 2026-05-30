/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 22:12:24 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/26 03:25:00 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <limits.h>

#define WAIT_UNTIL_FINISHED_MS UINT_MAX

static void	routine_one_philo(t_philo *philo)
{
	t_table	*table;
	int		fork;

	table = philo->table;
	fork = philo->left_fork_index;
	pthread_mutex_lock(&table->forks[fork]);
	print_status(table, philo->index + 1, "has taken a fork");
	time_sleep_ms(table, WAIT_UNTIL_FINISHED_MS);
	pthread_mutex_unlock(&table->forks[fork]);
}

static void	rest_phase(t_philo *philo)
{
	t_table	*table;
	int		philo_id;

	table = philo->table;
	philo_id = philo->index + 1;
	print_status(table, philo_id, "is sleeping");
	time_sleep_ms(table, (unsigned int)table->cfg.time_to_sleep);
	print_status(table, philo_id, "is thinking");
}

static void	stagger_start(t_philo *philo)
{
	unsigned int	delay;

	if (philo->index % 2 == 0)
		return ;
	delay = (unsigned int)philo->table->cfg.time_to_eat / 2;
	if (delay == 0)
		delay = 1;
	time_sleep_ms(philo->table, delay);
}

static void	routine_multi(t_philo *philo)
{
	t_table	*table;

	table = philo->table;
	stagger_start(philo);
	while (!table_is_finished(table))
	{
		if (!philo_meal_cycle(philo))
			break ;
		if (table_is_finished(table))
			break ;
		rest_phase(philo);
	}
}

void	*philo_routine(void *arg)
{
	t_philo	*philo;

	philo = arg;
	if (philo->table->cfg.number_of_philosophers == 1)
		routine_one_philo(philo);
	else
		routine_multi(philo);
	return (NULL);
}
