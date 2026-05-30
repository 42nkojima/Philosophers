/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_state.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 08:00:00 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/30 08:00:00 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	philo_record_meal_start(t_philo *philo)
{
	pthread_mutex_lock(&philo->table->state_mutex);
	philo->last_meal_ms = time_now_ms();
	pthread_mutex_unlock(&philo->table->state_mutex);
}

void	philo_increment_eat_count(t_philo *philo)
{
	pthread_mutex_lock(&philo->table->state_mutex);
	philo->eat_count++;
	pthread_mutex_unlock(&philo->table->state_mutex);
}

bool	philo_has_eaten_enough(t_philo *philo)
{
	t_table	*table;
	bool	enough;
	int		must;

	table = philo->table;
	must = table->cfg.number_of_times_each_philosopher_must_eat;
	if (must < 0)
		return (false);
	pthread_mutex_lock(&table->state_mutex);
	enough = philo->eat_count >= must;
	pthread_mutex_unlock(&table->state_mutex);
	return (enough);
}
