/* Copyright 2023 lbtrobit
 * Copyright 2017 Jason Williams (Wilba)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <stdint.h>

#define MACRO_ID_NULL  0xFF

typedef struct
{
    uint8_t macro_id;   // Current macro ID
    uint8_t dataPtr;    // Current position in the macro data
    uint8_t startPtr;   // Start position in the macro data
    uint8_t loopCount;  // Number of times to loop
    uint16_t delayms;   // delay code
} dynamic_macro_t;

typedef enum{
    MACRO_STATE_ABORTED,
    MACRO_STATE_RUNNING,
    MACRO_STATE_DONE
} macro_state_e;

void dynamic_macro_reset(void);
void dynamic_macro_loop_reset(void);
void dynamic_macro_pressed(uint8_t id);
void dynamic_macro_task(void);
