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

#include "stepper.h"
#include <stdlib.h>
#include <math.h>
#include <drivers/gpio.h>
#include <logging/log.h>

LOG_MODULE_DECLARE(zerhacker);

static int stepper_set_dir(const struct stepper *stepper, bool dir)
{
	int rc = gpio_pin_set(stepper->dir_dev, stepper->dir_pin, dir);

	if (rc != 0) {
		LOG_ERR("unable to set direciton pin");
		return -1;
	}

	return 0;
}

int stepper_enable(const struct stepper *stepper, bool enable)
{
	int rc = gpio_pin_set(stepper->enable_dev, stepper->enable_pin, enable);

	if (rc != 0) {
		LOG_ERR("unable to set enable pin");
		return -1;
	}

	return 0;
}

void stepper_move(struct stepper *stepper, int64_t target)
{
	struct ramp_info *ramp = &stepper->ramp;

	stepper->target_pos = target;
	int64_t steps = target - stepper->cur_pos;

	ramp->dir = steps >= 0;

	stepper_set_dir(stepper, ramp->dir);

	ramp->num_steps = abs(steps);
	ramp->acc_counter = 0;
	ramp->step_counter = 0;

	// calculate limits
	uint64_t s_alim = ramp->num_steps / 2;
	uint64_t s_vlim = stepper->max_v * stepper->max_v / stepper->max_accel;

	if (s_vlim < s_alim) {
		ramp->accel_steps = s_vlim;
	} else {
		ramp->accel_steps = s_alim;
	}
	// initial time delay
	uint64_t dt0 = sqrt(2.0f / stepper->max_accel) * 1000000.f;

	ramp->us = dt0;
	ramp->dt0 = dt0;

	stepper_enable(stepper, true);
	stepper->moving = true;
	k_timer_start(&stepper->timer, K_USEC(dt0), K_NO_WAIT);

	while (stepper->moving) {
		k_msleep(100);
	}
}

static void disable_step_pin(struct k_work *work)
{
	struct stepper *stepper = CONTAINER_OF(work, struct stepper, work);

	gpio_pin_set(stepper->step_dev, stepper->step_pin, 0);
}

void timer_clbk(struct k_timer *timer)
{
	struct stepper *stepper = CONTAINER_OF(timer, struct stepper, timer);
	struct ramp_info *ramp = &stepper->ramp;

	if (stepper->cur_pos == stepper->target_pos) {
		stepper->moving = false;
		return;
	}

	// do step
	gpio_pin_toggle(stepper->step_dev, stepper->step_pin);
	k_delayed_work_init(&stepper->work, disable_step_pin);
	k_delayed_work_submit(&stepper->work, K_USEC(10));

	ramp->step_counter++;
	if (ramp->dir) {
		stepper->cur_pos++;
	} else {
		stepper->cur_pos--;
	}

	if (ramp->step_counter <= ramp->accel_steps) {
		ramp->us = ramp->dt0 *
			   (sqrtf((ramp->step_counter + 1.0f)) -
			    sqrtf(ramp->step_counter));
	} else if (ramp->num_steps - ramp->step_counter > ramp->accel_steps) {
		// coast at constant speed
	} else {
		uint64_t rem = ramp->num_steps - ramp->step_counter;
		ramp->us = ramp->dt0 * (sqrtf((rem + 1.0f)) - sqrtf(rem));
	}

	k_timer_start(timer, K_USEC(ramp->us), K_NO_WAIT);
}

int stepper_init(struct stepper *stepper)
{
	int rc;

	stepper->enable_dev = device_get_binding(stepper->enable_node);
	stepper->dir_dev = device_get_binding(stepper->dir_node);
	stepper->step_dev = device_get_binding(stepper->step_node);

	if (stepper->enable_dev == NULL ||
	    stepper->dir_dev == NULL ||
	    stepper->step_dev == NULL) {
		LOG_ERR("unable to initialize stepper. Can not get gpio devs");
		return -1;
	}

	rc = gpio_pin_configure(stepper->enable_dev,
				stepper->enable_pin,
				GPIO_OUTPUT | GPIO_ACTIVE_LOW);

	if (rc != 0) {
		goto configure_error;
	}

	rc = gpio_pin_configure(stepper->dir_dev,
				stepper->dir_pin,
				GPIO_OUTPUT | GPIO_ACTIVE_HIGH);

	if (rc != 0) {
		goto configure_error;
	}

	rc = gpio_pin_configure(stepper->step_dev,
				stepper->step_pin,
				GPIO_OUTPUT | GPIO_ACTIVE_HIGH);

	if (rc != 0) {
		goto configure_error;
	}

	k_timer_init(&stepper->timer, timer_clbk, NULL);

	return 0;

configure_error:
	LOG_ERR("unable to configure stepper gpios");
	return rc;
}
