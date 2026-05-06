/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 10:56:56 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/06 15:40:26 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <sys/time.h>
#include <unistd.h>

#define SLEEP_SLICE_US 500

uint64_t	now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((uint64_t)tv.tv_sec * 1000ULL + (uint64_t)tv.tv_usec / 1000ULL);
}

void	precise_sleep_ms(unsigned int duration_ms, volatile bool *stop)
{
	uint64_t	deadline;

	if (duration_ms == 0 || *stop)
		return ;
	deadline = now_ms() + duration_ms;
	while (*stop == false)
	{
		if (now_ms() >= deadline)
			return ;
		usleep(SLEEP_SLICE_US);
	}
}
