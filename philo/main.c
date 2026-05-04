#include "philo.h"

int	main(void)
{
	t_config	config;

	config.number_of_philosophers = 0;
	config.time_to_die = 0;
	config.time_to_eat = 0;
	config.time_to_sleep = 0;
	config.number_of_times_each_philosopher_must_eat = 0;
	return (config.number_of_philosophers);
}
