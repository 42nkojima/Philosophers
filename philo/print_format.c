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

/**
 * print_format.c はログ1行の整形だけを行う
 */

/**
 * 文字列 s をログ用バッファ buf の末尾に追加していく
 * lenは「今 buf の何文字目まで使っているか」を表す
 * 呼び出し側は十分なサイズの buf を渡す前提
 */
static void	append_str(char *buf, size_t *len, const char *s)
{
	while (*s != '\0')
	{
		buf[*len] = *s;
		(*len)++;
		s++;
	}
}

/**
 * uint64_t の数値を 10進数のASCII文字列として buf に追加する
 * write は数値を直接出せないので、
 * 123 なら '1', '2', '3' という文字に変換する必要がある。
 *
 * n % 10 で下の桁から取れる。
 * そのまま前から buf に入れると逆順になるため、
 * 先に桁数を数えて、確保した範囲の後ろから数字を書き込む
 */
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

/**
 * subject のログ形式:
 *   timestamp_in_ms philosopher_id message\n
 *
 * 例:
 *   120 3 is eating
 *
 * timestamp は simulation 開始時刻からの差分
 * philo_id は表示用の id を受け取る想定
 */
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
