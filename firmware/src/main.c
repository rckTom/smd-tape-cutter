/*
 * smd tape cutter
 *
 * Copyright (c) Thomas Schmid, 2021
 *
 * Authors:
 *  Thomas Schmid <tom@lfence.de>
 *
 * This work is licensed under the terms of the GNU GPL, version 3.  See
 * the LICENSE file in the top-level directory.
 */

#include <zephyr.h>
#include <logging/log.h>
#include "zerhacker.h"

LOG_MODULE_REGISTER(zerhacker, CONFIG_LOG_DEFAULT_LEVEL);

void main(void)
{
	stepper_init(&stepper_1);
	stepper_init(&stepper_2);

	stepper_enable(&stepper_2, true);
	stepper_enable(&stepper_1, true);

	home_cutter();

	while (1) {
		k_msleep(1000);
	}
}