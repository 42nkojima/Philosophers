/*
 * print_status / print_death の外から見える振る舞いを確認する。
 * stdout は pipe + dup2 で一時的に差し替え、write(1, ...) の出力を読む。
 * timestamp は現在時刻依存なので完全一致させず、状態メッセージ部分を見る。
 */

#include "philo.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define OUT_BUF_SIZE 512

typedef struct s_capture
{
	int			saved_stdout;
	int			pipe_fd[2];
}				t_capture;

static void	init_table(t_table *table)
{
	table->start_time_ms = time_now_ms();
	table->finished = false;
	table->death_printed = false;
	pthread_mutex_init(&table->state_mutex, NULL);
	pthread_mutex_init(&table->print_mutex, NULL);
}

static void	destroy_table(t_table *table)
{
	pthread_mutex_destroy(&table->print_mutex);
	pthread_mutex_destroy(&table->state_mutex);
}

static int	capture_begin(t_capture *cap)
{
	fflush(stdout);
	if (pipe(cap->pipe_fd) == -1)
		return (-1);
	cap->saved_stdout = dup(STDOUT_FILENO);
	if (cap->saved_stdout == -1)
	{
		close(cap->pipe_fd[0]);
		close(cap->pipe_fd[1]);
		return (-1);
	}
	if (dup2(cap->pipe_fd[1], STDOUT_FILENO) == -1)
	{
		close(cap->saved_stdout);
		close(cap->pipe_fd[0]);
		close(cap->pipe_fd[1]);
		return (-1);
	}
	close(cap->pipe_fd[1]);
	return (0);
}

static ssize_t	capture_end(t_capture *cap, char *out, size_t out_size)
{
	ssize_t	n;

	fflush(stdout);
	if (dup2(cap->saved_stdout, STDOUT_FILENO) == -1)
		return (-1);
	close(cap->saved_stdout);
	n = read(cap->pipe_fd[0], out, out_size - 1);
	if (n < 0)
	{
		close(cap->pipe_fd[0]);
		return (-1);
	}
	out[n] = '\0';
	close(cap->pipe_fd[0]);
	return (n);
}

static int	count_substr(const char *s, const char *needle)
{
	int			count;
	const char	*p;
	size_t		needle_len;

	count = 0;
	p = s;
	needle_len = strlen(needle);
	while (1)
	{
		p = strstr(p, needle);
		if (p == NULL)
			break ;
		count++;
		p += needle_len;
	}
	return (count);
}

static int	test_print_status_outputs_message(void)
{
	t_table		table;
	t_capture	cap;
	char		out[OUT_BUF_SIZE];
	ssize_t		n;

	init_table(&table);
	if (capture_begin(&cap) == -1)
		return (printf("FAIL: capture_begin\n"), destroy_table(&table), 1);
	print_status(&table, 3, "is eating");
	n = capture_end(&cap, out, sizeof(out));
	destroy_table(&table);
	if (n <= 0)
		return (printf("FAIL: print_status wrote nothing\n"), 1);
	if (strstr(out, " 3 is eating\n") == NULL)
		return (printf("FAIL: bad status output: %s", out), 1);
	return (0);
}

static int	test_print_status_suppressed_after_finished(void)
{
	t_table		table;
	t_capture	cap;
	char		out[OUT_BUF_SIZE];
	ssize_t		n;

	init_table(&table);
	table.finished = true;
	if (capture_begin(&cap) == -1)
		return (printf("FAIL: capture_begin\n"), destroy_table(&table), 1);
	print_status(&table, 3, "is eating");
	n = capture_end(&cap, out, sizeof(out));
	destroy_table(&table);
	if (n != 0)
		return (printf("FAIL: status printed after finished: %s", out), 1);
	return (0);
}

static int	test_print_death_outputs_and_sets_flags(void)
{
	t_table		table;
	t_capture	cap;
	char		out[OUT_BUF_SIZE];
	ssize_t		n;
	bool		finished;
	bool		death_printed;

	init_table(&table);
	if (capture_begin(&cap) == -1)
		return (printf("FAIL: capture_begin\n"), destroy_table(&table), 1);
	print_death(&table, 2);
	n = capture_end(&cap, out, sizeof(out));
	finished = table.finished;
	death_printed = table.death_printed;
	destroy_table(&table);
	if (n <= 0)
		return (printf("FAIL: print_death wrote nothing\n"), 1);
	if (strstr(out, " 2 died\n") == NULL)
		return (printf("FAIL: bad death output: %s", out), 1);
	if (!finished || !death_printed)
		return (printf("FAIL: death flags were not set\n"), 1);
	return (0);
}

static int	test_print_death_only_once(void)
{
	t_table		table;
	t_capture	cap;
	char		out[OUT_BUF_SIZE];
	ssize_t		n;

	init_table(&table);
	if (capture_begin(&cap) == -1)
		return (printf("FAIL: capture_begin\n"), destroy_table(&table), 1);
	print_death(&table, 2);
	print_death(&table, 4);
	n = capture_end(&cap, out, sizeof(out));
	destroy_table(&table);
	if (n <= 0)
		return (printf("FAIL: print_death wrote nothing\n"), 1);
	if (count_substr(out, " died\n") != 1)
		return (printf("FAIL: death printed more than once: %s", out), 1);
	if (strstr(out, " 2 died\n") == NULL)
		return (printf("FAIL: first death output missing: %s", out), 1);
	return (0);
}

int	main(void)
{
	int	fail;

	fail = 0;
	fail += test_print_status_outputs_message();
	fail += test_print_status_suppressed_after_finished();
	fail += test_print_death_outputs_and_sets_flags();
	fail += test_print_death_only_once();
	if (fail == 0)
		printf("OK\n");
	return (fail != 0);
}
