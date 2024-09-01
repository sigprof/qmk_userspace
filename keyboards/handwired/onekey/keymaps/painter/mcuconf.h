// Copyright 2024 Sergey Vlasov <sigprof@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include_next <mcuconf.h>

#if defined(QMK_MCU_SERIES_STM32F4XX)
#    undef STM32_I2C_USE_I2C1
#    define STM32_I2C_USE_I2C1 TRUE
#    undef STM32_SPI_USE_SPI1
#    define STM32_SPI_USE_SPI1 TRUE
#elif defined(QMK_MCU_SERIES_RP2040)
#    undef RP_I2C_USE_I2C1
#    define RP_I2C_USE_I2C1 TRUE
#    undef RP_SPI_USE_SPI1
#    define RP_SPI_USE_SPI1 TRUE
#endif
