#if defined(CORE_CM7) || defined(CORE_CM4) 
#include "AdvancedAnalog.h"

#ifndef ARDUINO_AUDIO_DAC_H_
#define ARDUINO_AUDIO_DAC_H_

struct dac_descr_t;

class AudioDAC {
    private:
        size_t n_channels;
        dac_descr_t *descr;
        PinName dac_pins[AN_MAX_DAC_CHANNELS];

    public:
        template <typename ... T>
        AudioDAC(pin_size_t p0, T ... args): n_channels(0), descr(nullptr) {
            static_assert(sizeof ...(args) < AN_MAX_DAC_CHANNELS,
                    "A maximum of 1 channel is currently supported.");

            for (auto p : {p0, args...}) {
                dac_pins[n_channels++] = analogPinToPinName(p);
            }
        }
        ~AudioDAC();

        bool writeRequired();
        uint16_t* getWriteBuffer();
        void start();
        void writeCompleted();
        int init(uint32_t resolution, size_t n_samples=0);
        int stop();
};

#endif /* ARDUINO_AUDIO_DAC_H_ */
#endif