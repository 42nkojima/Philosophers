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

static bool	acquire_forks(t_philo *philo, int first_fork, int second_fork)
{
	t_table	*table;
	int		philo_id;

	table = philo->table;
	philo_id = philo->index + 1;
	pthread_mutex_lock(&table->forks[first_fork]);
	print_status(table, philo_id, "has taken a fork");
	if (table_is_finished(table))
	{
		pthread_mutex_unlock(&table->forks[first_fork]);
		philo_release_fork_reservation(table, first_fork, second_fork);
		return (false);
	}
	pthread_mutex_lock(&table->forks[second_fork]);
	print_status(table, philo_id, "has taken a fork");
	philo_record_meal_start(philo);
	return (true);
}

static void	release_forks(t_table *table, int first_fork, int second_fork)
{
	pthread_mutex_unlock(&table->forks[second_fork]);
	pthread_mutex_unlock(&table->forks[first_fork]);
	philo_release_fork_reservation(table, first_fork, second_fork);
}

static void	eat_phase(t_philo *philo, int first_fork, int second_fork,
		int philo_id)
{
	t_table	*table;

	table = philo->table;
	print_status(table, philo_id, "is eating");
	time_sleep_ms(table, (unsigned int)table->cfg.time_to_eat);
	philo_increment_eat_count(philo);
	release_forks(table, first_fork, second_fork);
}

bool	philo_meal_cycle(t_philo *philo)
{
	int	first_fork;
	int	second_fork;

	philo_order_forks(philo, &first_fork, &second_fork);
	if (!philo_wait_fork_reservation(philo, first_fork, second_fork))
		return (false);
	if (!acquire_forks(philo, first_fork, second_fork))
		return (philo_set_wants_to_eat(philo, false), false);
	eat_phase(philo, first_fork, second_fork, philo->index + 1);
	philo_set_wants_to_eat(philo, false);
	return (true);
}
