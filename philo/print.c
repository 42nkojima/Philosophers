/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 17:11:30 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/07 11:11:43 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_status(t_table *table, int philo_id, const char *msg)
{
	pthread_mutex_lock(&table->state_mutex);
	if (table->finished)
	{
		pthread_mutex_unlock(&table->state_mutex);
		return ;
	}
	pthread_mutex_lock(&table->print_mutex);
	print_write_line(table, philo_id, msg);
	pthread_mutex_unlock(&table->print_mutex);
	pthread_mutex_unlock(&table->state_mutex);
}

void	print_death_locked(t_table *table, int philo_id)
{
	if (table->death_printed)
		return ;
	table_finish_locked(table);
	table->death_printed = true;
	pthread_mutex_lock(&table->print_mutex);
	print_write_line(table, philo_id, "died");
	pthread_mutex_unlock(&table->print_mutex);
}

void	print_death(t_table *table, int philo_id)
{
	pthread_mutex_lock(&table->state_mutex);
	print_death_locked(table, philo_id);
	pthread_mutex_unlock(&table->state_mutex);
}
