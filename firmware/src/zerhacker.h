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

#ifndef ZERHACKER_ZERHACKER__H
#define ZERHACKER_ZERHACKER__H

#include "stepper.h"

extern struct stepper stepper_1;
extern struct stepper stepper_2;

void cut();
void cutter_up();
void cutter_down();
void home_cutter();

#endif