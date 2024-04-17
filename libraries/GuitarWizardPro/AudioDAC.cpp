#if defined(CORE_CM7) || defined(CORE_CM4) 
#include "Arduino.h"
#include "HALConfig.h"
#include "AudioDAC.h"

struct dac_descr_t 
{
    DAC_HandleTypeDef *dac;
    uint32_t  channel;
    DMA_HandleTypeDef dma;
    IRQn_Type dma_irqn;
    TIM_HandleTypeDef tim;
    uint32_t tim_trig;
    uint32_t resolution;
    uint32_t dmaudr_flag;
    uint16_t* buffer;
    uint16_t* writeBuffer;
    int size;
    bool writeRequired;
};

// NOTE: Both DAC channel descriptors share the same DAC handle.
static DAC_HandleTypeDef dac = {0};

static dac_descr_t dac_descr_all[] = 
{
    {&dac, DAC_CHANNEL_1, {DMA1_Stream4, {DMA_REQUEST_DAC1_CH1}}, DMA1_Stream4_IRQn, {TIM4},
        DAC_TRIGGER_T4_TRGO, DAC_ALIGN_12B_R, DAC_FLAG_DMAUDR1, nullptr,nullptr,0,false},
    {&dac, DAC_CHANNEL_2, {DMA1_Stream5, {DMA_REQUEST_DAC1_CH2}}, DMA1_Stream5_IRQn, {TIM5},
        DAC_TRIGGER_T5_TRGO, DAC_ALIGN_12B_R, DAC_FLAG_DMAUDR2, nullptr,nullptr,0,false},
};

static uint32_t DAC_RES_LUT[] = {    DAC_ALIGN_8B_R, DAC_ALIGN_12B_R, DAC_ALIGN_12B_R};

static uint32_t DAC_CHAN_LUT[] = {    DAC_CHANNEL_1, DAC_CHANNEL_2};

static dac_descr_t *dac_descr_get(uint32_t channel) {
    if (channel == DAC_CHANNEL_1) {
        return &dac_descr_all[0];
    } else if (channel == DAC_CHANNEL_2) {
        return &dac_descr_all[1];
    }
    return NULL;
}

static void dac_descr_deinit(dac_descr_t *descr) 
{
    if (descr != nullptr) 
    {
        HAL_TIM_Base_Stop(&descr->tim);
        HAL_DAC_Stop_DMA(descr->dac, descr->channel);
        __HAL_DAC_CLEAR_FLAG(descr->dac, descr->dmaudr_flag);

        delete descr->buffer;
    }
}
uint16_t* AudioDAC::getWriteBuffer()
{
    return descr->writeBuffer;
}
bool AudioDAC::writeRequired() 
{    
    return descr->writeRequired;
}

void AudioDAC::writeCompleted()
{
    descr->writeRequired=false;
}

void AudioDAC::start() 
{
    static uint32_t buf_count = 0;

    if (descr == nullptr) {
        return;
    }
       
    descr->writeBuffer =  descr->buffer+descr->size/2;       

        // Start DAC DMA.
    HAL_DAC_Start_DMA(descr->dac, descr->channel,
    (uint32_t *) descr->buffer, descr->size, descr->resolution);

    // Re/enable DMA double buffer mode.
    //HAL_NVIC_DisableIRQ(descr->dma_irqn);
    //  hal_dma_enable_dbm(&descr->dma, descr->dmabuf[0]->data(), descr->dmabuf[1]->data());
    //HAL_NVIC_EnableIRQ(descr->dma_irqn);

    // Start trigger timer.
    HAL_TIM_Base_Start(&descr->tim);
    descr->writeRequired=true;    
}

int AudioDAC::init(uint32_t resolution, size_t n_samples) 
{
    // Sanity checks.
    if (resolution >= AN_ARRAY_SIZE(DAC_RES_LUT) || descr != nullptr) 
    {
        return 0;
    }

 //   SCB_DisableDCache();
    // Configure DAC GPIO pins.
    for (size_t i=0; i<n_channels; i++) 
    {
        // Configure DAC GPIO pin.
        pinmap_pinout(dac_pins[i], PinMap_DAC);
    }

    uint32_t function = pinmap_function(dac_pins[0], PinMap_DAC);
    descr = dac_descr_get(DAC_CHAN_LUT[STM_PIN_CHANNEL(function) - 1]);
    if (descr == nullptr) 
    {
        return 0;
    }
    descr->buffer = new uint16_t[n_samples*2];
    descr->writeBuffer =  descr->buffer;
    descr->size = n_samples*2;
    descr->resolution = DAC_RES_LUT[resolution];

    // Init and config DMA.
    hal_dma_config(&descr->dma, descr->dma_irqn, DMA_MEMORY_TO_PERIPH);

    // Init and config DAC.
    hal_dac_config(descr->dac, descr->channel, descr->tim_trig);

    // Link channel's DMA handle to DAC handle
    if (descr->channel == DAC_CHANNEL_1) 
    {
        __HAL_LINKDMA(descr->dac, DMA_Handle1, descr->dma);
    } 
    else 
    {
        __HAL_LINKDMA(descr->dac, DMA_Handle2, descr->dma);
    }

    // Init and config the trigger timer.
    hal_tim_config(&descr->tim);
    return 1;
}

int AudioDAC::stop()
{
    if (descr != nullptr) {
        dac_descr_deinit(descr);
        descr = nullptr;
    }
    return 1;
}

AudioDAC::~AudioDAC()
{
    dac_descr_deinit(descr);
}

extern "C" 
{
    void DMA1_Stream4_IRQHandler() 
    {
        HAL_DMA_IRQHandler(&dac_descr_all[0].dma);
    }

    void DMA1_Stream5_IRQHandler() 
    {
        HAL_DMA_IRQHandler(&dac_descr_all[1].dma);
    }

    void DAC_DMAHalfConvCplt ( dac_descr_t* descr)
    {    
        descr->writeBuffer =  descr->buffer;     
        descr->writeRequired=true;  
    }

    void HAL_DAC_ConvHalfCpltCallbackCh1 (DAC_HandleTypeDef *hdac)
    {
        DAC_DMAHalfConvCplt(&dac_descr_all[0]);
    }

    void HAL_DAC_ConvHalfCpltCallbackCh2 (DAC_HandleTypeDef *hdac)
    {
        DAC_DMAHalfConvCplt(&dac_descr_all[1]);
    }

    void DAC_DMAConvCplt(  dac_descr_t* descr) 
    {
        descr->writeBuffer =  descr->buffer+descr->size/2;     
        descr->writeRequired=true;  
    }

    void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *dac) 
    {
        DAC_DMAConvCplt(&dac_descr_all[0]);
    }

    void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef *dac) 
    {
        DAC_DMAConvCplt(&dac_descr_all[1]);
    }
} // extern C
#endif