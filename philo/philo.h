/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 05:48:15 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/07 11:11:37 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdbool.h>
# include <stdint.h>

typedef struct s_config
{
	int				number_of_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				number_of_times_each_philosopher_must_eat;
}					t_config;

typedef struct s_table
{
	t_config		cfg;
	uint64_t		start_time_ms;
	bool			finished;
	bool			death_printed;
	pthread_mutex_t	state_mutex;
	pthread_mutex_t	print_mutex;
}					t_table;

/* parse.c */
int					parse_config(int ac, char **av, t_config *cfg);

/* time.c */
uint64_t			time_now_ms(void);
void				time_sleep_ms(unsigned int duration_ms,
						volatile bool *stop);

/* print_format.c */
void				print_write_line(t_table *table, int philo_id,
						const char *msg);

/* print.c */
void				print_status(t_table *table, int philo_id, const char *msg);
void				print_death(t_table *table, int philo_id);

#endif
