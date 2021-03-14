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

#include "zerhacker.h"
#include "devicetree.h"

#define STEPPER_MOVER DT_NODELABEL(stepper_mover)
#define STEPPER_CUTTER DT_NODELABEL(stepper_cutter)

struct stepper stepper_1 = {
	.enable_node = DT_GPIO_LABEL(STEPPER_CUTTER, enable_gpios),
	.dir_node = DT_GPIO_LABEL(STEPPER_CUTTER, dir_gpios),
	.step_node = DT_GPIO_LABEL(STEPPER_CUTTER, step_gpios),
	.enable_pin = DT_GPIO_PIN(STEPPER_CUTTER, enable_gpios),
	.dir_pin = DT_GPIO_PIN(STEPPER_CUTTER, dir_gpios),
	.step_pin = DT_GPIO_PIN(STEPPER_CUTTER, step_gpios),
	.max_accel = 8000.0,
	.max_v = 2000.0,
};

struct stepper stepper_2 = {
	.enable_node = DT_GPIO_LABEL(STEPPER_MOVER, enable_gpios),
	.dir_node = DT_GPIO_LABEL(STEPPER_MOVER, dir_gpios),
	.step_node = DT_GPIO_LABEL(STEPPER_MOVER, step_gpios),
	.enable_pin = DT_GPIO_PIN(STEPPER_MOVER, enable_gpios),
	.dir_pin = DT_GPIO_PIN(STEPPER_MOVER, dir_gpios),
	.step_pin = DT_GPIO_PIN(STEPPER_MOVER, step_gpios),
	.max_accel = 8000.0,
	.max_v = 2000.0,
};

void cut()
{
	cutter_down();
	cutter_up();
}

void cutter_down()
{
	stepper_move(&stepper_1, -230);
}

void cutter_up()
{
	stepper_move(&stepper_1, -10);
}

void home_cutter()
{
	float old_speed = stepper_1.max_v;

	stepper_1.max_v = 50;
	stepper_move(&stepper_1, 450);
	stepper_1.cur_pos = 0;
	stepper_move(&stepper_1, -10);
	stepper_1.max_v = old_speed;
}
