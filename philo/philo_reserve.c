/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_reserve.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 22:05:00 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/29 22:05:00 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static bool	philo_has_priority(t_philo *candidate, t_philo *philo)
{
	if (candidate->last_meal_ms < philo->last_meal_ms)
		return (true);
	if (candidate->last_meal_ms > philo->last_meal_ms)
		return (false);
	return (candidate->index < philo->index);
}

void	philo_order_forks(t_philo *philo, int *first_fork, int *second_fork)
{
	if (philo->left_fork_index < philo->right_fork_index)
	{
		*first_fork = philo->left_fork_index;
		*second_fork = philo->right_fork_index;
	}
	else
	{
		*first_fork = philo->right_fork_index;
		*second_fork = philo->left_fork_index;
	}
}

static bool	try_reserve_forks(t_philo *philo, int first_fork,
		int second_fork)
{
	t_table	*table;
	t_philo	*left;
	t_philo	*right;
	int		count;

	table = philo->table;
	count = table->cfg.number_of_philosophers;
	left = &table->philos[(philo->index + count - 1) % count];
	right = &table->philos[(philo->index + 1) % count];
	pthread_mutex_lock(&table->state_mutex);
	if (table->finished || table->fork_reserved[first_fork]
		|| table->fork_reserved[second_fork]
		|| (left->wants_to_eat && philo_has_priority(left, philo))
		|| (right->wants_to_eat && philo_has_priority(right, philo)))
	{
		pthread_mutex_unlock(&table->state_mutex);
		return (false);
	}
	table->fork_reserved[first_fork] = true;
	table->fork_reserved[second_fork] = true;
	pthread_mutex_unlock(&table->state_mutex);
	return (true);
}

void	philo_release_fork_reservation(t_table *table, int first_fork,
		int second_fork)
{
	pthread_mutex_lock(&table->state_mutex);
	table->fork_reserved[first_fork] = false;
	table->fork_reserved[second_fork] = false;
	pthread_mutex_unlock(&table->state_mutex);
}

bool	philo_wait_fork_reservation(t_philo *philo, int first_fork,
		int second_fork)
{
	philo_set_wants_to_eat(philo, true);
	while (!table_is_finished(philo->table))
	{
		if (try_reserve_forks(philo, first_fork, second_fork))
			return (true);
		time_sleep_ms(philo->table, 1);
	}
	philo_set_wants_to_eat(philo, false);
	return (false);
}
