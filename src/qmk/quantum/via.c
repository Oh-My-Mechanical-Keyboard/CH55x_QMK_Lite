/* Copyright 2019 Jason Williams (Wilba)
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

#include "quantum.h"
#include "via.h"
#include "dynamic_keymap.h"
#include <Arduino.h>

static uint16_t eeprom_save_interval;

// Called by QMK core to initialize dynamic keymaps etc.
void via_init(void) {
    // If the EEPROM has the magic, the data is good.
    // OK to load from EEPROM.
    if (!(eeprom_read_byte(VIA_EEPROM_MAGIC_ADDR) == VIA_MAGIC_CODE)) {
        eeconfig_init_via();
    }
}

void eeconfig_init_via(void) {
    // This resets the keymaps in EEPROM to what is in flash.
    dynamic_keymap_reset();
    // This resets the macros in EEPROM to nothing.
    dynamic_macro_reset();
#ifdef RGB_MATRIX_ENABLE
    // This resets the RGB Matrix settings in EEPROM to nothing.
    rgb_matrix_reset();
#endif //RGB_MATRIX_ENABLE
    // Save the magic number last, in case saving was interrupted
    eeprom_write_byte(VIA_EEPROM_MAGIC_ADDR, VIA_MAGIC_CODE);
}

void bootloader_jump(void) {
    USB_CTRL = 0;
    EA = 0;	        // Disabling all interrupts is required.
    TMOD = 0;
    __asm__ ("lcall #0x3800"); // Jump to bootloader code
    while (1);
}

// Called by QMK core to process VIA-specific keycodes.
bool process_record_via(uint16_t keycode, keyrecord_t *record) {
    // Handle macros
    if (record->event.pressed) {
        if (keycode >= QK_MACRO && keycode <= QK_MACRO_MAX) {
            dynamic_macro_pressed(keycode - QK_MACRO);
            return false;
        }
    }

    return true;
}

#ifdef RGB_MATRIX_ENABLE 
void via_qmk_rgb_matrix_command(uint8_t *data)
{
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id = &(data[0]);
    uint8_t *channel_id = &(data[1]);
    uint8_t *value_id = &(data[2]);
    uint8_t *value_data = &(data[3]);

    switch (*command_id) {
        case id_custom_set_value:
        case id_custom_save:
            if (timer_elapsed32(eeprom_save_interval) >= VIA_EEPROM_SAVE_INTERVAL) {
                switch (*value_id) {
                    case id_qmk_rgb_matrix_effect:
                        rgb_matrix_set_mode(*value_data);
                        break;
#   ifdef RGB_EFFECTS_PLUS
                    case id_qmk_rgb_matrix_brightness:
                        rgb_matrix_set_val(*value_data);
                        break;
                    case id_qmk_rgb_matrix_effect_speed:
                        rgb_matrix_set_speed(*value_data);
                        break;
                    case id_qmk_rgb_matrix_color:
                        rgb_matrix_set_hs(*value_data, *(value_data + 1));
                        break;
#   endif //RGB_EFFECTS_PLUS
                    default:
                        if (*value_id >= id_qmk_rgb_matrix_color_red && *value_id <= id_qmk_rgb_matrix_color_blue) {
                            eeprom_write_byte(VIA_EEPROM_CUSTOM_RGB_MATRIX_ADDR + *value_id - 5, *value_data);
                        }
                        break;
                }
                eeprom_save_interval = timer_read();
            }
            break;

        case id_custom_get_value:
            switch (*value_id) {
                case id_qmk_rgb_matrix_effect:
                    *value_data = eeprom_read_byte(RGB_MATRIX_EEPROM_ADDR_EFFECT);
                    break;
#   ifdef RGB_EFFECTS_PLUS
                case id_qmk_rgb_matrix_brightness:
                    *value_data = eeprom_read_byte(RGB_MATRIX_EEPROM_ADDR_VAL);
                    break;
                case id_qmk_rgb_matrix_effect_speed:
                    *value_data = eeprom_read_byte(RGB_MATRIX_EEPROM_ADDR_SPEED);
                    break;
                case id_qmk_rgb_matrix_color:
                    break;
#   endif //RGB_EFFECTS_PLUS
                default:
                    if (*value_id >= id_qmk_rgb_matrix_color_red && *value_id <= id_qmk_rgb_matrix_color_blue) {
                        *value_data = eeprom_read_byte(VIA_EEPROM_CUSTOM_RGB_MATRIX_ADDR + *value_id - 5);
                    }
                    break;
            }
            break;
    }
}
#endif //RGB_MATRIX_ENABLE

void via_custom_value_command(uint8_t *data) {
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id = &(data[0]);
    uint8_t *channel_id = &(data[1]);
    uint8_t *value_id = &(data[2]);
    uint8_t *value_data = &(data[3]);

    if (*channel_id == id_qmk_macro_loop_channel) {
        switch (*command_id) {
            case id_custom_set_value:
            case id_custom_save:
                eeprom_write_byte(VIA_EEPROM_CUSTOM_MACRO_LOOP_ADDR + *value_id - 1, *value_data);
                break;
            case id_custom_get_value:
                *value_data = eeprom_read_byte(VIA_EEPROM_CUSTOM_MACRO_LOOP_ADDR + *value_id - 1);
                break;
        }
        return;
    } else if (*channel_id == id_qmk_macro_system) {
        switch (*command_id) {
            case id_custom_set_value:
            case id_custom_save:
                switch (*value_id) {
                    case id_system_eeprom_reset:
                        eeconfig_init_via();
                        break;
                    case id_system_bootloader_jump:
                        bootloader_jump();
                        break;
                }
                break;
            case id_custom_get_value:
                *value_data = 0;
                break;
        }
        return;
    }
#ifdef RGB_MATRIX_ENABLE 
    else if (*channel_id == id_qmk_rgb_matrix_channel) {
        via_qmk_rgb_matrix_command(data);
        return;
    }
#endif //RGB_MATRIX_ENABLE

    *command_id = id_unhandled;
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    uint8_t *command_id   = &(data[0]);
    uint8_t *command_data = &(data[1]);

    switch (*command_id) {
#ifdef RGB_MATRIX_ENABLE
        case id_signalrgb_stream_leds: {
            rgb_matrix_signalrgb_set_leds(data);
            break;
        }
        case id_signalrgb_effect_enable: {
            rgb_matrix_set_mode_noeeprom(RGB_MATRIX_SIGNAL_RGB);
            break;
        }
        case id_signalrgb_effect_disable: {
            rgb_matrix_reload_mode();
            break;
        }
#endif
        case id_get_protocol_version: {
            command_data[0] = VIA_PROTOCOL_VERSION >> 8;
            command_data[1] = VIA_PROTOCOL_VERSION & 0xFF;
            break;
        }
        case id_get_keyboard_value:
        case id_set_keyboard_value:
            break;
        case id_dynamic_keymap_get_keycode: {
            uint16_t keycode = dynamic_keymap_get_keycode(command_data[1], command_data[2]);
            command_data[3]  = keycode >> 8;
            command_data[4]  = keycode & 0xFF;
            break;
        }
        case id_dynamic_keymap_set_keycode: {
            dynamic_keymap_set_keycode(command_data[1], command_data[2], (command_data[3] << 8) | command_data[4]);
            break;
        }
        case id_dynamic_keymap_reset: {
            dynamic_keymap_reset();
            break;
        }
        case id_custom_set_value:
        case id_custom_get_value:
        case id_custom_save:
            via_custom_value_command(data);
            break;
        case id_eeprom_reset:
        case id_bootloader_jump:
            break;
        case id_dynamic_keymap_macro_get_count: {
            command_data[0] = DYNAMIC_KEYMAP_MACRO_COUNT;
            break;
        }
        case id_dynamic_keymap_macro_get_buffer_size: {
            uint16_t size   = DYNAMIC_KEYMAP_MACRO_EEPROM_SIZE;
            command_data[0] = size >> 8;
            command_data[1] = size & 0xFF;
            break;
        }
        case id_dynamic_keymap_macro_get_buffer: {
            for (uint8_t i = 0; i < command_data[2]; i++) {
                command_data[3 + i] = eeprom_read_byte(DYNAMIC_KEYMAP_MACRO_EEPROM_ADDR + command_data[1] + i);
            }
            break;
        }
        case id_dynamic_keymap_macro_set_buffer: {
            for (uint8_t i = 0; i < command_data[2]; i++) {
                eeprom_write_byte(DYNAMIC_KEYMAP_MACRO_EEPROM_ADDR + command_data[1] + i, command_data[3 + i]);
            }
            break;
        }
        case id_dynamic_keymap_macro_reset: {
            dynamic_macro_reset();
            break;
        }
        case id_dynamic_keymap_get_layer_count: {
            command_data[0] = 1;
            break;
        }
        case id_dynamic_keymap_get_buffer: {
            for (uint8_t i = 0; i < command_data[2]; i++) {
                command_data[3 + i] = eeprom_read_byte(DYNAMIC_KEYMAP_EEPROM_ADDR + command_data[1] + i);
            }
            break;
        }
        case id_dynamic_keymap_set_buffer: {
            for (uint8_t i = 0; i < command_data[2]; i++) {
                eeprom_write_byte(DYNAMIC_KEYMAP_EEPROM_ADDR + command_data[1] + i, command_data[3 + i]);
            }
            break;
        }
#ifdef ENCODER_ENABLE
        case id_dynamic_keymap_get_encoder: {
            uint16_t keycode = dynamic_keymap_get_encoder(command_data[1], command_data[2] != 0);
            command_data[3]  = keycode >> 8;
            command_data[4]  = keycode & 0xFF;
            break;
        }
        case id_dynamic_keymap_set_encoder: {
            dynamic_keymap_set_encoder(command_data[1], command_data[2] != 0, (command_data[3] << 8) | command_data[4]);
            break;
        }
#endif
        default: {
            // The command ID is not known
            // Return the unhandled state
            *command_id = id_unhandled;
            break;
        }
    }

    // Return the same buffer, optionally with values changed
    // (i.e. returning state to the host, or the unhandled state).
    raw_hid_send(data, length);
}
