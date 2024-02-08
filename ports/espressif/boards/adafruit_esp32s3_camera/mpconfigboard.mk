USB_VID = 0x239A
USB_PID = 0x8118
USB_PRODUCT = "Camera"
USB_MANUFACTURER = "Adafruit"

IDF_TARGET = esp32s3

CIRCUITPY_ESP_FLASH_SIZE = 4MB
CIRCUITPY_ESP_FLASH_MODE = qio
CIRCUITPY_ESP_FLASH_FREQ = 80m

CIRCUITPY_ESP_PSRAM_SIZE = 2MB
CIRCUITPY_ESP_PSRAM_MODE = qio
CIRCUITPY_ESP_PSRAM_FREQ = 80m
FLASH_SDKCONFIG = esp-idf-config/sdkconfig-4MB-1ota.defaults

CIRCUITPY_AUDIOBUSIO = 0
CIRCUITPY_CANIO = 0
CIRCUITPY_ESPCAMERA = 1
CIRCUITPY_FRAMEBUFFERIO = 0
CIRCUITPY_KEYPAD = 0
CIRCUITPY_ONEWIREIO = 0
CIRCUITPY_PARALLELDISPLAYBUS = 0
CIRCUITPY_RGBMATRIX = 0
CIRCUITPY_ROTARYIO = 0

OPTIMIZATION_FLAGS = -Os
