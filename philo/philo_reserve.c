/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_reserve.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 22:05:00 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/30 11:48:03 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

#define TURN_EVEN 0
#define TURN_ODD 1
#define TURN_LAST 2

static int	philo_meal_group(t_philo *philo)
{
	int	count;

	count = philo->table->cfg.number_of_philosophers;
	if (count % 2 == 0)
		return (philo->index % 2);
	if (philo->index == count - 1)
		return (TURN_LAST);
	return (philo->index % 2);
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

/*
Atomically reserve both forks under state_mutex.
The active turn is structural, not based on another philosopher's state.
*/
static bool	try_reserve_forks(t_philo *philo, int first_fork, int second_fork)
{
	t_table	*table;
	int		group;

	table = philo->table;
	group = philo_meal_group(philo);
	pthread_mutex_lock(&table->state_mutex);
	if (table->finished || table->meal_turn != group
		|| table->fork_reserved[first_fork]
		|| table->fork_reserved[second_fork])
	{
		pthread_mutex_unlock(&table->state_mutex);
		return (false);
	}
	table->fork_reserved[first_fork] = true;
	table->fork_reserved[second_fork] = true;
	table->active_reservations++;
	pthread_mutex_unlock(&table->state_mutex);
	return (true);
}

void	philo_release_fork_reservation(t_philo *philo, int first_fork,
		int second_fork)
{
	t_table	*table;

	table = philo->table;
	pthread_mutex_lock(&table->state_mutex);
	table->fork_reserved[first_fork] = false;
	table->fork_reserved[second_fork] = false;
	if (table->active_reservations > 0)
		table->active_reservations--;
	if (table->active_reservations == 0)
	{
		if (table->cfg.number_of_philosophers % 2 == 0)
			table->meal_turn = 1 - table->meal_turn;
		else if (table->meal_turn == TURN_LAST)
			table->meal_turn = TURN_EVEN;
		else
			table->meal_turn++;
	}
	pthread_mutex_unlock(&table->state_mutex);
}

bool	philo_wait_fork_reservation(t_philo *philo, int first_fork,
		int second_fork)
{
	while (!table_is_finished(philo->table) && !philo_has_eaten_enough(philo))
	{
		if (try_reserve_forks(philo, first_fork, second_fork))
			return (true);
		time_sleep_ms(philo->table, 1);
	}
	return (false);
}
