#ifndef AUDIO_GUARD
#define AUDIO_GUARD

#include "driverlib/MSP430FR5xx_6xx/driverlib.h"

#ifndef DECODER_GUARD
#include "decoder/decoder.h"
#endif

#ifndef NEURAL_NETWORK_PARAMS_GUARD
#include "neural_network_parameters.h"
#endif

#define SAMPLING_FREQUENCY              2000
#define __SYSTEM_FREQUENCY_MHZ__        16000000
#define FFT_WINDOW_SIZE                 256
#define FFT_RESULT_SIZE                 64

#define SAMPLES_LENGTH      2048
#define AUDIO_PORT_SEL0     P1SEL0
#define AUDIO_PORT_SEL1     P1SEL1
#define MIC_INPUT_PIN       BIT3

#define INPUT_BUFFER_LENGTH       960

typedef struct Audio_configParams
{
    uint16_t bufferSize;
    uint16_t sampleRate;
} Audio_configParams;

void runSampling_Audio(void);
void Audio_setupCollect(Audio_configParams *audioConfig);
void Audio_startCollect(void);
void Audio_stopCollect(void);
void Audio_shutdownCollect(void);
void fft_interrupt(void);

DMA_initParam dma0Config;
Audio_configParams gAudioConfig;

#pragma PERSISTENT(dataRecorded);
static uint16_t dataRecorded[SAMPLES_LENGTH] = {0};

#endif
