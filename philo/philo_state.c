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

void	philo_set_wants_to_eat(t_philo *philo, bool value)
{
	pthread_mutex_lock(&philo->table->state_mutex);
	philo->wants_to_eat = value;
	pthread_mutex_unlock(&philo->table->state_mutex);
}

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
