/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_format.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 17:11:27 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/07 11:11:40 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdint.h>
#include <unistd.h>

#define LOG_BUF_SIZE 128

static void	append_str(char *buf, size_t *len, const char *s)
{
	while (*s != '\0')
	{
		buf[*len] = *s;
		(*len)++;
		s++;
	}
}

static void	append_uint64(char *buf, size_t *len, uint64_t n)
{
	uint64_t	tmp;
	size_t		digits;
	size_t		end;

	if (n == 0)
	{
		buf[*len] = '0';
		(*len)++;
		return ;
	}
	tmp = n;
	digits = 0;
	while (tmp > 0)
	{
		tmp /= 10;
		digits++;
	}
	end = *len + digits;
	*len = end;
	while (n > 0)
	{
		end--;
		buf[end] = '0' + (n % 10);
		n /= 10;
	}
}

void	print_write_line(t_table *table, int philo_id, const char *msg)
{
	char		buf[LOG_BUF_SIZE];
	size_t		len;
	uint64_t	timestamp;

	len = 0;
	timestamp = time_now_ms() - table->start_time_ms;
	append_uint64(buf, &len, timestamp);
	buf[len++] = ' ';
	append_uint64(buf, &len, (uint64_t)philo_id);
	buf[len++] = ' ';
	append_str(buf, &len, msg);
	buf[len++] = '\n';
	write(1, buf, len);
}
