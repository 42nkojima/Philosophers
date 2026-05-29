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

static bool	philo_prio_over(t_philo *a, t_philo *b)
{
	if (a->last_meal_ms < b->last_meal_ms)
		return (true);
	if (a->last_meal_ms > b->last_meal_ms)
		return (false);
	return (a->id < b->id);
}

void	philo_fork_order(t_philo *philo, int *first, int *second)
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

static bool	try_reserve_forks(t_philo *philo, int first, int second)
{
	t_table	*table;
	t_philo	*left;
	t_philo	*right;
	int		count;

	table = philo->table;
	count = table->cfg.number_of_philosophers;
	left = &table->philos[(philo->id + count - 1) % count];
	right = &table->philos[(philo->id + 1) % count];
	pthread_mutex_lock(&table->state_mutex);
	if (table->finished || table->fork_reserved[first]
		|| table->fork_reserved[second]
		|| (left->hungry && philo_prio_over(left, philo))
		|| (right->hungry && philo_prio_over(right, philo)))
	{
		pthread_mutex_unlock(&table->state_mutex);
		return (false);
	}
	table->fork_reserved[first] = true;
	table->fork_reserved[second] = true;
	pthread_mutex_unlock(&table->state_mutex);
	return (true);
}

void	philo_unreserve(t_table *table, int first, int second)
{
	pthread_mutex_lock(&table->state_mutex);
	table->fork_reserved[first] = false;
	table->fork_reserved[second] = false;
	pthread_mutex_unlock(&table->state_mutex);
}

bool	philo_wait_reserve(t_philo *philo, int first, int second)
{
	pthread_mutex_lock(&philo->table->state_mutex);
	philo->hungry = true;
	pthread_mutex_unlock(&philo->table->state_mutex);
	while (!table_is_finished(philo->table))
	{
		if (try_reserve_forks(philo, first, second))
			return (true);
		time_sleep_ms(philo->table, 1);
	}
	pthread_mutex_lock(&philo->table->state_mutex);
	philo->hungry = false;
	pthread_mutex_unlock(&philo->table->state_mutex);
	return (false);
}
