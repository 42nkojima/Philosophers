/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 05:48:15 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/25 16:30:00 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdbool.h>
# include <stdint.h>

typedef struct s_config
{
	int					number_of_philosophers;
	int					time_to_die;
	int					time_to_eat;
	int					time_to_sleep;
	int					number_of_times_each_philosopher_must_eat;
}						t_config;

typedef struct s_philo	t_philo;

typedef struct s_table
{
	t_config			cfg;
	uint64_t			start_time_ms;
	bool				finished;
	bool				death_printed;
	pthread_mutex_t		state_mutex;
	pthread_mutex_t		print_mutex;
	t_philo				*philos;
	pthread_mutex_t		*forks;
}						t_table;

typedef struct s_philo
{
	int					id;
	uint64_t			last_meal_ms;
	int					eat_count;
	int					left_fork_index;
	int					right_fork_index;
	t_table				*table;
}						t_philo;

/* parse.c */
int						parse_config(int ac, char **av, t_config *cfg);

/* table.c */
int						table_init(t_table *table, const t_config *cfg);
void					table_destroy(t_table *table);

/* time.c */
uint64_t				time_now_ms(void);
void					time_sleep_ms(t_table *table,
							unsigned int duration_ms);

/* print_format.c */
void					print_write_line(t_table *table, int philo_id,
							const char *msg);

/* print.c */
void					print_status(t_table *table, int philo_id,
							const char *msg);
void					print_death(t_table *table, int philo_id);

/* philo_routine.c */
void					*philo_routine(void *arg);

/* monitor.c */
void					*monitor_routine(void *arg);

#endif
