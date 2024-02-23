#include "shared-bindings/board/__init__.h"

#include "supervisor/board.h"

STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // Analog
    { MP_OBJ_NEW_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_GPIO_AD_02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_GPIO_AD_01) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_GPIO_AD_00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_GPIO_AD_05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_GPIO_AD_10) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_GPIO_AD_08) },

    // Digital
    { MP_OBJ_NEW_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_GPIO_09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO_09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_GPIO_10) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO_10) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_GPIO_13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_GPIO_12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_GPIO_SD_00) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_GPIO_SD_01) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_GPIO_SD_02) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_GPIO_11) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_GPIO_08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_GPIO_07) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_GPIO_06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_GPIO_05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_GPIO_04) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO_03) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_GPIO_03) },

    // SPI
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_GPIO_AD_06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_GPIO_AD_03) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_GPIO_AD_04) },

    // I2C
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO_01) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO_02) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_GPIO_SD_05) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_STEMMA_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },

    { MP_ROM_QSTR(MP_QSTR_I2S_WORD_SELECT), MP_ROM_PTR(&pin_GPIO_06) }, // D10
    { MP_ROM_QSTR(MP_QSTR_I2S_WSEL), MP_ROM_PTR(&pin_GPIO_06) }, // D10

    { MP_ROM_QSTR(MP_QSTR_I2S_BIT_CLOCK), MP_ROM_PTR(&pin_GPIO_07) }, // D9
    { MP_ROM_QSTR(MP_QSTR_I2S_BCLK), MP_ROM_PTR(&pin_GPIO_07) }, // D9

    { MP_ROM_QSTR(MP_QSTR_I2S_DATA), MP_ROM_PTR(&pin_GPIO_04) }, // D12
    { MP_ROM_QSTR(MP_QSTR_I2S_DOUT), MP_ROM_PTR(&pin_GPIO_04) }, // D12

    // Sequencer flatcable connector
    { MP_OBJ_NEW_QSTR(MP_QSTR_ROW_1), MP_ROM_PTR(&pin_GPIO_13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ROW_2), MP_ROM_PTR(&pin_GPIO_12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ROW_3), MP_ROM_PTR(&pin_GPIO_SD_04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ROW_4), MP_ROM_PTR(&pin_GPIO_11) },
    // These are only availble on 2.0 beta boards. Later POTs are only on mux
    { MP_OBJ_NEW_QSTR(MP_QSTR_POT_1), MP_ROM_PTR(&pin_GPIO_AD_04) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POT_2), MP_ROM_PTR(&pin_GPIO_AD_03) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_LED_DATA), MP_ROM_PTR(&pin_GPIO_SD_05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL_1), MP_ROM_PTR(&pin_GPIO_AD_09) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL_2), MP_ROM_PTR(&pin_GPIO_AD_08) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL_3), MP_ROM_PTR(&pin_GPIO_AD_07) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL_4), MP_ROM_PTR(&pin_GPIO_AD_06) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL_5), MP_ROM_PTR(&pin_GPIO_AD_05) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_COL_6), MP_ROM_PTR(&pin_GPIO_AD_04) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
