/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 17:11:30 by nkojima           #+#    #+#             */
/*   Updated: 2026/05/07 11:11:43 by nkojima          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * print.c はログ出力時の共有状態チェックと mutex 制御を担当する。
 *
 * 実際の文字列整形と write は print_format.c の print_write_line が行う。
 * ここではログを出してよいか判断し、ログ行が混ざらないように lock する。
 *
 * mutex の取得順序は必ず:
 *
 *   state_mutex -> print_mutex
 *
 * にする。
 */

/**
 * 通常の状態ログを出力する。
 *
 * finished が true の場合、simulation は終了済みなので通常ログは出さない。
 * finished は共有状態なので、読むときも state_mutex で守る。
 *
 * print_write_line は stdout に write するため、
 * 複数 thread のログ行が混ざらないよう print_mutex で守る。
 */
void	print_status(t_table *table, int philo_id, const char *msg)
{
	pthread_mutex_lock(&table->state_mutex);
	if (table->finished)
	{
		pthread_mutex_unlock(&table->state_mutex);
		return ;
	}
	pthread_mutex_lock(&table->print_mutex);
	print_write_line(table, philo_id, msg);
	pthread_mutex_unlock(&table->print_mutex);
	pthread_mutex_unlock(&table->state_mutex);
}

/**
 * 死亡ログを出力する。
 *
 * died は simulation 終了時にも必ず一度だけ出す必要がある。
 * そのため通常ログのように finished が true なら return、とはしない。
 *
 * death_printed が true なら、すでに死亡ログを出しているので何もしない。
 * まだ出していなければ、finished と death_printed を true にしてから出力する。
 */
/**
 * 死亡ログを出力する（state_mutex 保持中に呼ぶ）。
 */
void	print_death_locked(t_table *table, int philo_id)
{
	if (table->death_printed)
		return ;
	table->finished = true;
	table->death_printed = true;
	pthread_mutex_lock(&table->print_mutex);
	print_write_line(table, philo_id, "died");
	pthread_mutex_unlock(&table->print_mutex);
}

void	print_death(t_table *table, int philo_id)
{
	pthread_mutex_lock(&table->state_mutex);
	print_death_locked(table, philo_id);
	pthread_mutex_unlock(&table->state_mutex);
}
