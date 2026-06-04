#include "philo.h"
#include <stdio.h>

static int	fail_fork(t_table *table, int id, int left, int right)
{
	if (table->philos[id].left_fork_index != left
		|| table->philos[id].right_fork_index != right)
		return (printf("FAIL: n=%d id=%d forks got (%d,%d) expected (%d,%d)\n",
				table->cfg.number_of_philosophers, id,
				table->philos[id].left_fork_index,
				table->philos[id].right_fork_index, left, right), 1);
	return (0);
}

static int	test_fork_indices(void)
{
	t_table		table;
	t_config	cfg;
	int			fail;

	cfg = (t_config){5, 800, 200, 200, -1};
	if (table_init(&table, &cfg) != 0)
		return (printf("FAIL: table_init n=5\n"), 1);
	fail = fail_fork(&table, 0, 0, 1);
	fail += fail_fork(&table, 4, 4, 0);
	table_destroy(&table);
	cfg = (t_config){2, 800, 200, 200, -1};
	if (table_init(&table, &cfg) != 0)
		return (printf("FAIL: table_init n=2\n"), 1);
	fail += fail_fork(&table, 0, 0, 1);
	fail += fail_fork(&table, 1, 1, 0);
	table_destroy(&table);
	cfg = (t_config){1, 800, 200, 200, -1};
	if (table_init(&table, &cfg) != 0)
		return (printf("FAIL: table_init n=1\n"), 1);
	fail += fail_fork(&table, 0, 0, 0);
	table_destroy(&table);
	return (fail);
}

static int	philo_fields_ok(t_table *table, int id)
{
	return (table->philos[id].index == id && table->philos[id].table == table
		&& table->philos[id].eat_count == 0
		&& table->philos[id].last_meal_ms == table->start_time_ms);
}

static int	test_table_initial_fields(void)
{
	t_table		table;
	t_config	cfg;
	int			fail;

	cfg = (t_config){3, 800, 200, 200, -1};
	if (table_init(&table, &cfg) != 0)
		return (printf("FAIL: table_init n=3\n"), 1);
	fail = 0;
	if (table.finished || table.death_printed || table.meal_turn != 0
		|| table.active_reservations != 0)
	{
		printf("FAIL: table state should start empty\n");
		fail = 1;
	}
	if (!philo_fields_ok(&table, 0))
	{
		printf("FAIL: philo 0 initial fields\n");
		fail = 1;
	}
	if (!philo_fields_ok(&table, 2))
	{
		printf("FAIL: philo 2 initial fields\n");
		fail = 1;
	}
	table_destroy(&table);
	return (fail);
}

int	main(void)
{
	int	fail;

	fail = 0;
	fail += test_fork_indices();
	fail += test_table_initial_fields();
	if (fail == 0)
		printf("OK\n");
	return (fail != 0);
}
