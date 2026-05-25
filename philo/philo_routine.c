/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 22:12:24 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/26 02:49:50 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <limits.h>

/* time_sleep_ms: finished で抜ける。deadline は UINT_MAX で実質無効化 (n==1) */
#define WAIT_UNTIL_FINISHED_MS UINT_MAX

/*
n != 1 は今は return NULL で良い
died は出さない (monitorに任せる)
*/

static void	routine_one_philo(t_philo *philo)
{
	t_table	*table;
	int		fork;

	table = philo->table;
	fork = philo->left_fork_index;
	pthread_mutex_lock(&table->forks[fork]);
	print_status(table, philo->id + 1, "has taken a fork");
	time_sleep_ms(table, WAIT_UNTIL_FINISHED_MS);
	pthread_mutex_unlock(&table->forks[fork]);
}

void	*philo_routine(void *arg)
{
	t_philo	*philo;

	philo = arg;
	if (philo->table->cfg.number_of_philosophers != 1)
		return (NULL);
	routine_one_philo(philo);
	return (NULL);
}
