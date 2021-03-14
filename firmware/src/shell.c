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
#include <logging/log.h>
#include <shell/shell.h>
#include <stdlib.h>

LOG_MODULE_DECLARE(zerhacker);

static int shell_stepper_move_abs(const struct shell *shell,
				  size_t argc,
				  char **argv)
{
	int id = strtol(*(argv + 1), NULL, 10);
	int pos = strtol(*(argv + 2), NULL, 10);

	if (id < 1 || id > 2) {
		LOG_ERR("stepper id not supported");
		return -1;
	}

	struct stepper *stepper;

	if (id == 1) {
		stepper = &stepper_1;
	} else if (id == 2) {
		stepper = &stepper_2;
	}

	stepper_move(stepper, pos);
	return 0;
}

static int shell_stepper_move_rel(const struct shell *shell,
				  size_t argc,
				  char **argv)
{
	int id = strtol(*(argv + 1), NULL, 10);
	int pos = strtol(*(argv + 2), NULL, 10);

	if (id < 1 || id > 2) {
		LOG_ERR("stepper id not supported");
		return -1;
	}

	struct stepper *stepper;

	if (id == 1) {
		stepper = &stepper_1;
	} else if (id == 2) {
		stepper = &stepper_2;
	}

	stepper_move(stepper, stepper->cur_pos + pos);
	return 0;
}

static int shell_hacker_job(const struct shell *shell, int argc, char **argv)
{
	int length = strtol(*(argv + 1), NULL, 10);
	int count = strtol(*(argv + 2), NULL, 10);

	float pos = 0;

	stepper_2.cur_pos = 0;
	cutter_up();

	for (; count > 0; count--) {
		pos += 800.f / 30.f * length;
		stepper_move(&stepper_2, (int64_t) pos);
		cut();
	}

	return 0;
}

static int shell_disable_stepper(const struct shell *shell,
				 int argc,
				 char **argv)
{
	stepper_enable(&stepper_1, false);
	stepper_enable(&stepper_2, false);

	return 0;
}

static int shell_move_knife(const struct shell *shell, int argc, char **argv)
{
	const char *in = *(argv + 1);

	if (strcmp(in, "up") == 0) {
		cutter_up();
	} else if (strcmp(in, "down") == 0) {
		cutter_down();
	} else {
		return -1;
	}

	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_hacker,
			       SHELL_CMD_ARG(move_rel, NULL,
					     "move stepper. arg1: stepper number, arg2: position",
					     shell_stepper_move_rel, 3,
					     0),
			       SHELL_CMD_ARG(move_abs, NULL,
					     "move to absolute coordinates. arg1: stepper number, arg2: position",
					     shell_stepper_move_abs, 3, 0),
			       SHELL_CMD_ARG(knife, NULL,
					     "position knife. arg1: up or down",
					     shell_move_knife, 2, 0),
			       SHELL_CMD_ARG(hack, NULL,
					     "do job. arg1: length, arg2: amount ",
					     shell_hacker_job, 3, 0),
			       SHELL_CMD_ARG(disable_stepper, NULL,
					     "disable all steppers",
					     shell_disable_stepper, 1, 0),
			       SHELL_SUBCMD_SET_END
			       );

SHELL_CMD_REGISTER(hacker, &sub_hacker, "hacker control", NULL);
