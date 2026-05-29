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
		philo_unreserve(table, first, second);
		return (false);
	}
	pthread_mutex_lock(&table->forks[second]);
	print_status(table, id, "has taken a fork");
	pthread_mutex_lock(&table->state_mutex);
	philo->last_meal_ms = time_now_ms();
	pthread_mutex_unlock(&table->state_mutex);
	return (true);
}

static void	release_forks(t_table *table, int first, int second)
{
	pthread_mutex_unlock(&table->forks[second]);
	pthread_mutex_unlock(&table->forks[first]);
	philo_unreserve(table, first, second);
}

static void	clear_hungry(t_philo *philo)
{
	pthread_mutex_lock(&philo->table->state_mutex);
	philo->hungry = false;
	pthread_mutex_unlock(&philo->table->state_mutex);
}

static void	eat_phase(t_philo *philo, int first, int second, int id)
{
	t_table	*table;

	table = philo->table;
	print_status(table, id, "is eating");
	time_sleep_ms(table, (unsigned int)table->cfg.time_to_eat);
	pthread_mutex_lock(&table->state_mutex);
	philo->eat_count++;
	pthread_mutex_unlock(&table->state_mutex);
	release_forks(table, first, second);
}

bool	philo_meal_cycle(t_philo *philo)
{
	int		first;
	int		second;

	philo_fork_order(philo, &first, &second);
	if (!philo_wait_reserve(philo, first, second))
		return (false);
	if (!acquire_forks(philo, first, second))
		return (clear_hungry(philo), false);
	eat_phase(philo, first, second, philo->id + 1);
	clear_hungry(philo);
	return (true);
}
