/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_meal.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 03:25:00 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/26 04:00:00 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <unistd.h>

#define FORK_WAIT_US 500

static void	fork_indices(t_philo *philo, int *first, int *second)
{
	if (philo->left_fork_index < philo->right_fork_index)
	{
		*first = philo->left_fork_index;
		*second = philo->right_fork_index;
	}
	else
	{
		*first = philo->right_fork_index;
		*second = philo->left_fork_index;
	}
}

static bool	lock_fork_or_wait(t_philo *philo, int fork_index)
{
	t_table	*table;

	table = philo->table;
	while (!table_is_finished(table))
	{
		if (pthread_mutex_trylock(&table->forks[fork_index]) == 0)
			return (true);
		pthread_mutex_lock(&philo->table->state_mutex);
		philo->last_meal_ms = time_now_ms();
		pthread_mutex_unlock(&philo->table->state_mutex);
		usleep(FORK_WAIT_US);
	}
	return (false);
}

static bool	acquire_forks(t_philo *philo, int first, int second)
{
	t_table	*table;
	int		id;

	table = philo->table;
	id = philo->id + 1;
	if (!lock_fork_or_wait(philo, first))
		return (false);
	print_status(table, id, "has taken a fork");
	if (table_is_finished(table))
	{
		pthread_mutex_unlock(&table->forks[first]);
		return (false);
	}
	if (!lock_fork_or_wait(philo, second))
	{
		pthread_mutex_unlock(&table->forks[first]);
		return (false);
	}
	print_status(table, id, "has taken a fork");
	pthread_mutex_lock(&philo->table->state_mutex);
	philo->last_meal_ms = time_now_ms();
	pthread_mutex_unlock(&philo->table->state_mutex);
	return (true);
}

static void	release_forks(t_table *table, int first, int second)
{
	pthread_mutex_unlock(&table->forks[second]);
	pthread_mutex_unlock(&table->forks[first]);
}

bool	philo_meal_cycle(t_philo *philo)
{
	t_table	*table;
	int		first;
	int		second;
	int		id;

	table = philo->table;
	id = philo->id + 1;
	fork_indices(philo, &first, &second);
	if (!acquire_forks(philo, first, second))
		return (false);
	print_status(table, id, "is eating");
	time_sleep_ms(table, (unsigned int)table->cfg.time_to_eat);
	pthread_mutex_lock(&table->state_mutex);
	philo->eat_count++;
	pthread_mutex_unlock(&table->state_mutex);
	release_forks(table, first, second);
	return (true);
}
