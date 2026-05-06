#include "philo.h"
#include <stdio.h>

static int	expect_ok(char **av, int ac, t_config expected)
{
	t_config	cfg;

	if (parse_config(ac, av, &cfg) != 0)
		return (printf("FAIL: expected ok for av[1]=%s\n", av[1]), 1);
	if (cfg.number_of_philosophers != expected.number_of_philosophers
		|| cfg.time_to_die != expected.time_to_die
		|| cfg.time_to_eat != expected.time_to_eat
		|| cfg.time_to_sleep != expected.time_to_sleep
		|| cfg.number_of_times_each_philosopher_must_eat
		!= expected.number_of_times_each_philosopher_must_eat)
		return (printf("FAIL: values differ: got {%d,%d,%d,%d,%d}"
				" expected {%d,%d,%d,%d,%d}\n",
				cfg.number_of_philosophers, cfg.time_to_die,
				cfg.time_to_eat, cfg.time_to_sleep,
				cfg.number_of_times_each_philosopher_must_eat,
				expected.number_of_philosophers, expected.time_to_die,
				expected.time_to_eat, expected.time_to_sleep,
				expected.number_of_times_each_philosopher_must_eat), 1);
	return (0);
}

static int	expect_ng(char **av, int ac)
{
	t_config	cfg;

	if (parse_config(ac, av, &cfg) != -1)
		return (printf("FAIL: expected ng\n"), 1);
	return (0);
}

int	main(void)
{
	int	fail;

	fail = 0;
	fail += expect_ok((char *[]){"./philo", "5", "800", "200", "200"}, 5,
			(t_config){5, 800, 200, 200, -1});
	fail += expect_ok((char *[]){"./philo", "5", "800", "200", "200", "7"}, 6,
			(t_config){5, 800, 200, 200, 7});
	fail += expect_ng((char *[]){"./philo", "+5", "800", "200", "200"}, 5);
	fail += expect_ng((char *[]){"./philo"}, 1);
	fail += expect_ng((char *[]){"./philo", "5", "800", "200"}, 4);
	fail += expect_ng((char *[]){"./philo", "5", "800", "200", "200", "7", "x"},
			7);
	fail += expect_ng((char *[]){"./philo", "0", "800", "200", "200"}, 5);
	fail += expect_ng((char *[]){"./philo", "-5", "800", "200", "200"}, 5);
	fail += expect_ng((char *[]){"./philo", "+", "800", "200", "200"}, 5);
	fail += expect_ng((char *[]){"./philo", "", "800", "200", "200"}, 5);
	fail += expect_ng((char *[]){"./philo", "5x", "800", "200", "200"}, 5);
	fail += expect_ng((char *[]){"./philo", "2147483648", "800", "200", "200"},
			5);
	fail += expect_ok((char *[]){"./philo", "2147483647", "800", "200", "200"},
			5, (t_config){2147483647, 800, 200, 200, -1});
	fail += expect_ng((char *[]){"./philo", "5", "800", "200", "200", "0"}, 6);
	if (fail == 0)
		printf("OK\n");
	return (fail != 0);
}
