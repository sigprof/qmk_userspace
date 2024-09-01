// Copyright 2024 Sergey Vlasov <sigprof@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define QUANTUM_PAINTER_DISPLAY_TIMEOUT 0

#if defined(QMK_MCU_SERIES_STM32F4XX)

// SPI pins
#    define SPI_DRIVER SPID1
#    define SPI_SCK_PIN B3
#    define SPI_MOSI_PIN B5
#    define SPI_MISO_PIN B4

// Other display pins
#    define DISPLAY_RST_PIN B8
#    define DISPLAY_DC_PIN A15
#    define DISPLAY_CS_PIN B9

#elif defined(QMK_MCU_SERIES_RP2040)

// SPI pins
#    define SPI_DRIVER SPID1
#    define SPI_SCK_PIN GP14
#    define SPI_MOSI_PIN GP15
#    define SPI_MISO_PIN GP12

// Other display pins
#    define DISPLAY_RST_PIN GP11
#    define DISPLAY_DC_PIN GP13
#    define DISPLAY_CS_PIN GP10

#endif
