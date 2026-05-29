/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table_state.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 08:00:00 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/30 08:00:00 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

bool	table_is_finished(t_table *table)
{
	bool	finished;

	pthread_mutex_lock(&table->state_mutex);
	finished = table->finished;
	pthread_mutex_unlock(&table->state_mutex);
	return (finished);
}

void	table_finish_locked(t_table *table)
{
	table->finished = true;
}

void	table_finish(t_table *table)
{
	pthread_mutex_lock(&table->state_mutex);
	table_finish_locked(table);
	pthread_mutex_unlock(&table->state_mutex);
}
