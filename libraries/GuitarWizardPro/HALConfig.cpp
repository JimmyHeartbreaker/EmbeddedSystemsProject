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

#include "HALConfig.h"

static uint32_t hal_tim_freq(TIM_HandleTypeDef *tim) {
    // NOTE: If a APB1/2 prescaler is set, respective timers clock should
    // be doubled, however, it seems the right timer clock is not doubled.
    if (((uint32_t) tim->Instance & (~0xFFFFUL)) ==  D2_APB1PERIPH_BASE) {
        return HAL_RCC_GetPCLK1Freq() * ((RCC->D2CFGR & RCC_D2CFGR_D2PPRE1) ? 2 : 1);
    } else {
        return HAL_RCC_GetPCLK2Freq() * ((RCC->D2CFGR & RCC_D2CFGR_D2PPRE2) ? 2 : 1);
    }
}

int hal_tim_config(TIM_HandleTypeDef *tim) {
    

    tim->Init.Period                = 1;
    tim->Init.Prescaler             = 3749;
    tim->Init.CounterMode           = TIM_COUNTERMODE_UP;
    tim->Init.ClockDivision         = TIM_CLOCKDIVISION_DIV1;
    tim->Init.RepetitionCounter     = 0;
    tim->Init.AutoReloadPreload     = TIM_AUTORELOAD_PRELOAD_ENABLE;

    TIM_MasterConfigTypeDef sConfig = {0};
    sConfig.MasterOutputTrigger     = TIM_TRGO_UPDATE;
    sConfig.MasterOutputTrigger2    = TIM_TRGO2_RESET;
    sConfig.MasterSlaveMode         = TIM_MASTERSLAVEMODE_ENABLE;

    if (tim->Instance == TIM1) {
        __HAL_RCC_TIM1_CLK_ENABLE();
    } else if (tim->Instance == TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();
    } else if (tim->Instance == TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE();
    } else if (tim->Instance == TIM4) {
        __HAL_RCC_TIM4_CLK_ENABLE();
    } else if (tim->Instance == TIM5) {
        __HAL_RCC_TIM5_CLK_ENABLE();
    } else if (tim->Instance == TIM6) {
        __HAL_RCC_TIM6_CLK_ENABLE();
    }

    // Init and config the timer.
    __HAL_TIM_CLEAR_FLAG(tim, TIM_FLAG_UPDATE);
    if ((HAL_TIM_PWM_Init(tim) != HAL_OK)
    || (HAL_TIMEx_MasterConfigSynchronization(tim, &sConfig) != HAL_OK)) {
        return -1;
    }
    return 0;
}

int hal_dma_config(DMA_HandleTypeDef *dma, IRQn_Type irqn, uint32_t direction) {
    // Enable DMA clock
    __HAL_RCC_DMA1_CLK_ENABLE();

    // DMA Init
    dma->Init.Mode                  = DMA_CIRCULAR;
    dma->Init.Priority              = DMA_PRIORITY_VERY_HIGH;
    dma->Init.Direction             = direction;
    dma->Init.FIFOMode              = DMA_FIFOMODE_ENABLE;
    dma->Init.FIFOThreshold         = DMA_FIFO_THRESHOLD_FULL;
    dma->Init.MemInc                = DMA_MINC_ENABLE;
    dma->Init.PeriphInc             = DMA_PINC_DISABLE;
    dma->Init.MemBurst              = DMA_MBURST_SINGLE;
    dma->Init.PeriphBurst           = DMA_PBURST_SINGLE;
    dma->Init.MemDataAlignment      = DMA_MDATAALIGN_HALFWORD;
    dma->Init.PeriphDataAlignment   = DMA_PDATAALIGN_HALFWORD;

    if (HAL_DMA_DeInit(dma) != HAL_OK
     || HAL_DMA_Init(dma) != HAL_OK) {
        return -1;
    }

    // NVIC configuration for DMA Input data interrupt.
    HAL_NVIC_SetPriority(irqn, 1, 0);
    HAL_NVIC_EnableIRQ(irqn);

    return 0;
}





int hal_dac_config(DAC_HandleTypeDef *dac, uint32_t channel, uint32_t trigger) {
    // DAC init
    if (dac->Instance == NULL) {
        // Enable DAC clock
        __HAL_RCC_DAC12_CLK_ENABLE();

        dac->Instance = DAC1;
        if (HAL_DAC_DeInit(dac) != HAL_OK
         || HAL_DAC_Init(dac) != HAL_OK) {
            return -1;
        }
    }

    DAC_ChannelConfTypeDef sConfig = {0};
    sConfig.DAC_Trigger         = trigger;
    sConfig.DAC_OutputBuffer    = DAC_OUTPUTBUFFER_DISABLE;
    sConfig.DAC_UserTrimming    = DAC_TRIMMING_FACTORY;
    sConfig.DAC_SampleAndHold   = DAC_SAMPLEANDHOLD_DISABLE;
    sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
    if (HAL_DAC_ConfigChannel(dac, &sConfig, channel) != HAL_OK) {
        return -1;
    }

    return 0;
}


#endif