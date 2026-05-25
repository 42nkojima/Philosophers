#include "philo.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

static uint64_t	wall_us(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)tv.tv_usec);
}

static int	test_time_now_ms_monotonic(void)
{
	uint64_t	a;
	uint64_t	b;

	a = time_now_ms();
	usleep(2000);
	b = time_now_ms();
	if (b < a)
		return (printf("FAIL: time_now_ms not monotonic\n"), 1);
	return (0);
}

static int	test_precise_sleep_zero_and_stop(void)
{
	t_table		table;
	uint64_t	t0;
	uint64_t	dt;

	pthread_mutex_init(&table.state_mutex, NULL);
	pthread_mutex_lock(&table.state_mutex);
	table.finished = false;
	pthread_mutex_unlock(&table.state_mutex);
	t0 = wall_us();
	time_sleep_ms(&table, 0);
	dt = wall_us() - t0;
	if (dt > 3000)
	{
		pthread_mutex_destroy(&table.state_mutex);
		return (printf("FAIL: time_sleep_ms(0) took too long\n"), 1);
	}
	pthread_mutex_lock(&table.state_mutex);
	table.finished = true;
	pthread_mutex_unlock(&table.state_mutex);
	t0 = wall_us();
	time_sleep_ms(&table, 10000);
	dt = wall_us() - t0;
	pthread_mutex_destroy(&table.state_mutex);
	if (dt > 3000)
		return (printf("FAIL: time_sleep_ms(..., finished) blocked\n"), 1);
	return (0);
}

static int	test_precise_sleep_duration(void)
{
	t_table		table;
	uint64_t	t0;
	uint64_t	dt;

	pthread_mutex_init(&table.state_mutex, NULL);
	pthread_mutex_lock(&table.state_mutex);
	table.finished = false;
	pthread_mutex_unlock(&table.state_mutex);
	t0 = wall_us();
	time_sleep_ms(&table, 50);
	dt = wall_us() - t0;
	pthread_mutex_destroy(&table.state_mutex);
	if (dt < 40000)
		return (printf("FAIL: time_sleep_ms(50) too short (%llu us)\n",
				(unsigned long long)dt), 1);
	if (dt > 250000)
		return (printf("FAIL: time_sleep_ms(50) too long (%llu us)\n",
				(unsigned long long)dt), 1);
	return (0);
}

int	main(void)
{
	int	fail;

	fail = 0;
	fail += test_time_now_ms_monotonic();
	fail += test_precise_sleep_zero_and_stop();
	fail += test_precise_sleep_duration();
	if (fail == 0)
		printf("OK\n");
	return (fail != 0);
}
