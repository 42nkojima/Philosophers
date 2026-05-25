/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_meal.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 03:25:00 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/26 03:25:00 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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

static bool	acquire_forks(t_philo *philo, int first, int second)
{
	t_table	*table;
	int		id;

	table = philo->table;
	id = philo->id + 1;
	pthread_mutex_lock(&table->forks[first]);
	print_status(table, id, "has taken a fork");
	if (table_is_finished(table))
	{
		pthread_mutex_unlock(&table->forks[first]);
		return (false);
	}
	pthread_mutex_lock(&table->forks[second]);
	print_status(table, id, "has taken a fork");
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
	pthread_mutex_lock(&table->state_mutex);
	philo->last_meal_ms = time_now_ms();
	pthread_mutex_unlock(&table->state_mutex);
	print_status(table, id, "is eating");
	time_sleep_ms(table, (unsigned int)table->cfg.time_to_eat);
	release_forks(table, first, second);
	return (true);
}
