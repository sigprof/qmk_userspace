// Copyright 2024 Sergey Vlasov <sigprof@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define HAL_USE_I2C TRUE
#define HAL_USE_SPI TRUE
#define SPI_USE_WAIT TRUE

#include_next <halconf.h>
