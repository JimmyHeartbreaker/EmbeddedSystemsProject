#if defined(CORE_CM7) || defined(CORE_CM4) 
/*
  This file is part of the Arduino_AdvancedAnalog library.
  Copyright (c) 2023 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __HAL_CONFIG_H__
#define __HAL_CONFIG_H__

#include "Arduino.h"

int hal_tim_config(TIM_HandleTypeDef *tim);
int hal_dma_config(DMA_HandleTypeDef *dma, IRQn_Type irqn, uint32_t direction);

int hal_dac_config(DAC_HandleTypeDef *dac, uint32_t channel, uint32_t trigger);

#endif  // __HAL_CONFIG_H__
#endif