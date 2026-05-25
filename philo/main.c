/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 05:48:09 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/25 22:22:36 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
parse -> table_init -> threads -> join -> destroy
init 失敗時は destroy しない(mutex 失敗パス注意)
*/
int	main(int ac, char **av)
{
	t_config	cfg;
	t_table		table;
	pthread_t	mon;
	pthread_t	philo_th;

	if (parse_config(ac, av, &cfg) == -1)
		return (1);
	if (table_init(&table, &cfg) == -1)
		return (1);
	pthread_create(&mon, NULL, monitor_routine, &table);
	if (cfg.number_of_philosophers == 1)
		pthread_create(&philo_th, NULL, philo_routine, &table.philos[0]);
	pthread_join(mon, NULL);
	if (cfg.number_of_philosophers == 1)
		pthread_join(philo_th, NULL);
	table_destroy(&table);
	return (0);
}
