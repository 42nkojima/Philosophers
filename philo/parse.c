/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 05:15:32 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/06 05:47:55 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <limits.h>
#include <stdbool.h>

static bool	is_digit(char c)
{
	return ('0' <= c && c <= '9');
}

static int	parse_pos_int(const char *s, int *out)
{
	int		i;
	long	n;

	if (!s || !*s)
		return (-1);
	n = 0;
	i = 0;
	while (s[i])
	{
		if (!is_digit(s[i]))
			return (-1);
		n = n * 10 + (s[i] - '0');
		if (n > INT_MAX)
			return (-1);
		i++;
	}
	if (n <= 0)
		return (-1);
	*out = (int)n;
	return (0);
}

int	parse_config(int ac, char **av, t_config *cfg)
{
	if (ac != 5 && ac != 6)
		return (-1);
	if (parse_pos_int(av[1], &cfg->number_of_philosophers) == -1)
		return (-1);
	if (parse_pos_int(av[2], &cfg->time_to_die) == -1)
		return (-1);
	if (parse_pos_int(av[3], &cfg->time_to_eat) == -1)
		return (-1);
	if (parse_pos_int(av[4], &cfg->time_to_sleep) == -1)
		return (-1);
	cfg->number_of_times_each_philosopher_must_eat = -1;
	if (ac == 6)
	{
		if (parse_pos_int(av[5],
				&cfg->number_of_times_each_philosopher_must_eat) == -1)
			return (-1);
	}
	return (0);
}
