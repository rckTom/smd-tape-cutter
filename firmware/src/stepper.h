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

#ifndef ZERHACKER_STEPPER__H
#define ZERHACKER_STEPPER__H

#include <stdint.h>
#include <stdbool.h>
#include <zephyr.h>

struct ramp_info {
	uint64_t accel_steps;
	uint64_t num_steps;

	uint64_t acc_counter;
	uint64_t step_counter;

	bool dir;
	uint64_t us;
	uint64_t dt0;
};

struct stepper {
	bool moving;
	const char *enable_node;
	const char *dir_node;
	const char *step_node;
	const int enable_pin;
	const int dir_pin;
	const int step_pin;

	int64_t cur_pos, target_pos;

	float max_v, max_accel;

	struct k_delayed_work work;
	struct k_timer timer;

	const struct device *enable_dev;
	const struct device *dir_dev;
	const struct device *step_dev;

	struct ramp_info ramp;
};

void stepper_move(struct stepper *stepper, int64_t target);
int stepper_init(struct stepper *stepper);
int stepper_enable(const struct stepper *stepper, bool enable);

#endif

