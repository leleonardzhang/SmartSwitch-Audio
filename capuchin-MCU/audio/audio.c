/*
 * audio.c
 *
 *  Created on: May 4, 2019
 *      Author: Yubo
 *      Edit: Le
 */

//******************************************************************************
//!
//!  This file includes the voltage/audio sampling.
//!  We developed our audio/voltage sampling module based on
//!  BOOSTXL-AUDIO Audio Record and Playback Example
//!  Example code can be downloaded from
//!  https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP-EXP430FR5994/latest/index_FDS.html
//!
//!  Description: Sound detection demo on MSP-EXP430FR5994
//!               using ADMP401
//!
//!               (LED may not work as mentioned here, because LED-control was changed very frequently
//!               during experiments and I am not sure in what status it is now)
//!
//!               the MCU samples a 300ms audio signal, and then uses threshold method to detect if an event happens
//!               Red LED1 indicating the MCU is sampling
//!
//!               After the red LED is off, the MCU starts processing data (event detecting)
//!               Green LED2 indicates an event is captured, green LED lasts for 0.25s
//!
//!               NOTE: This demo requires the ADMP401
//!
//!                  MSP430FR5994               Audio ADMP401
//!               -----------------             -----------------
//!              |             P1.3|<--------- | MIC OUT         |
//!              |             P6.2|---------> | MIC PWR         |
//!              |              GND|---------- | MIC GND         |
//!              |-----------------|           |-----------------|
//!
//******************************************************************************

#include "audio.h"

dtype input_buffer[960] = {0};

/* when sampling the voltage, it is unreliable if only sample once
 * we sample VCC_sample_num times, and only use the last average_num data points,
 * then average them and get the final voltage
 * VCC_sample_num is used in the Audio-sample initialization part when setting up DMA transferSize
*/


void runSampling_Audio(void){

//    /* turn on RED LED when starting sampling, it should be turned off in DMA Interrupt */
//    if(DEBUG == 0 && DebugOntheFly == 1)
//        GPIO_setOutputHighOnPin(REDLED)); // P1OUT |= BIT0;


    /* Initialize the microphone for recording */
    gAudioConfig.bufferSize = SAMPLES_LENGTH;
    gAudioConfig.sampleRate = SAMPLING_FREQUENCY;

    /* setup audio-sampling configuration */
    Audio_setupCollect(&gAudioConfig);

    /* Start the recording by enabling the timer */
    Audio_startCollect();

    __bis_SR_register(LPM3_bits + GIE);

    Audio_shutdownCollect();
}


//******************************************************************************
// Functions
//******************************************************************************
/* Function that powers up the external microphone and starts sampling
 * the microphone output.
 * The ADC is triggered to sample using the Timer module
 * Then the data is moved via DMA. The device would only wake-up once
 * the DMA is done. */
void Audio_setupCollect(Audio_configParams * audioConfig)
{
    Timer_A_initUpModeParam upConfig = {0};
        upConfig.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
        upConfig.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
        upConfig.timerPeriod = (__SYSTEM_FREQUENCY_MHZ__ / audioConfig->sampleRate) - 1;
        upConfig.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
        upConfig.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
        upConfig.timerClear = TIMER_A_DO_CLEAR;
        upConfig.startTimer = false;


    Timer_A_initCompareModeParam compareConfig = {0};
        compareConfig.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
        compareConfig.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
        compareConfig.compareOutputMode = TIMER_A_OUTPUTMODE_TOGGLE_RESET;
        compareConfig.compareValue = ((__SYSTEM_FREQUENCY_MHZ__ / audioConfig->sampleRate) / 2) - 1;


    // Initialize Timer_A channel 1 to be used as ADC12 trigger
    // Initialize TACCR0 (period register) __SYSTEM_FREQUENCY_MHZ__/sampleRate = NUM
    // Simple counter with no interrupt. 0...NUM = NUM counts/sample
    Timer_A_initUpMode(TIMER_A0_BASE, &upConfig);

    // Initialize TA0CCR1 to generate trigger clock output, reset/set mode
    Timer_A_initCompareMode(TIMER_A0_BASE, &compareConfig);


    P1SEL1 |= BIT3; // Configure P1.3 for ADC
    P1SEL0 |= BIT3;

    // For safety, protect RMW Cpu instructions
    DMA_disableTransferDuringReadModifyWrite();

    // Initialize the DMA. Using DMA channel 1.
    dma0Config.channelSelect = DMA_CHANNEL_1;
    dma0Config.transferModeSelect = DMA_TRANSFER_SINGLE;

    dma0Config.transferSize = audioConfig->bufferSize;   // how many times of sampling

    dma0Config.triggerSourceSelect = DMA_TRIGGERSOURCE_26;
    dma0Config.transferUnitSelect = DMA_SIZE_SRCWORD_DSTWORD;
    dma0Config.triggerTypeSelect = DMA_TRIGGER_RISINGEDGE;


    DMA_init(&dma0Config);

    DMA_setSrcAddress(DMA_CHANNEL_1,
                      (uint32_t) &ADC12MEM0,
                      DMA_DIRECTION_UNCHANGED);

    DMA_setDstAddress(DMA_CHANNEL_1,
                      (uint32_t) (&dataRecorded),
                      DMA_DIRECTION_INCREMENT);


    // Configure ADC
    ADC12CTL0 &= ~ADC12ENC;           // Disable conversions to configure ADC12
    // Turn on ADC, sample 32 clock cycles =~ 2us
    ADC12CTL0 = ADC12ON + ADC12SHT0_3;

    // Use sample timer, rpt single chan 0, use MODOSC, TA0 timer channel 1
    ADC12CTL1 = ADC12SHP + ADC12CONSEQ_2 + ADC12SHS_1;

    // set input to ADC, (AVCC/AVSS ref), sequence end bit set
    ADC12MCTL0 = ADC12INCH_3 | ADC12VRSEL_0 | ADC12EOS;

    // Enable ADC to convert when a TA0 edge is generated
    ADC12CTL0 |= ADC12ENC;
}

/*--------------------------------------------------------------------------*/
/* Start collecting audio samples in ping-pong buffers */
void Audio_startCollect(void)
{
    // Enable DMA channel 1 interrupt
    DMA_enableInterrupt(DMA_CHANNEL_1);

    // Enable the DMA0 to start receiving triggers when ADC sample available
    DMA_enableTransfers(DMA_CHANNEL_1);

    // Start TA0 timer to begin audio data collection
    Timer_A_clear(TIMER_A0_BASE);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

/*--------------------------------------------------------------------------*/
/* Stop collecting audio samples in buffers */
void Audio_stopCollect(void)
{
    Timer_A_stop(TIMER_A0_BASE);

    ADC12_B_disableConversions(ADC12_B_BASE, ADC12_B_COMPLETECONVERSION);

    // Disable DMA channel and interrupt
    DMA_disableTransfers(DMA_CHANNEL_1);
    DMA_disableInterrupt(DMA_CHANNEL_1);

}

/*--------------------------------------------------------------------------*/
/* Shut down the audio collection peripherals*/
void Audio_shutdownCollect(void)
{
    /*
     * DMA finishes
     */
    // Disable the dma transfer
    DMA_disableTransfers(DMA_CHANNEL_1);
    // Disable DMA channel 1 interrupt
    DMA_disableInterrupt(DMA_CHANNEL_1);
//    GPIO_setOutputLowOnPin(REDLED)); // P1OUT &= ~BIT0;  // turn off LED

    /*
     * audio stop collect
     */
    Timer_A_stop(TIMER_A0_BASE);
    ADC12_B_disableConversions(ADC12_B_BASE, ADC12_B_COMPLETECONVERSION);
    // Disable DMA channel and interrupt
    DMA_disableTransfers(DMA_CHANNEL_1);
    DMA_disableInterrupt(DMA_CHANNEL_1);

    ADC12_B_disable(ADC12_B_BASE);
}

void fft_interrupt(void){
    uint16_t i = 0;
    for (i = 0; i < ((SAMPLES_LENGTH / FFT_WINDOW_SIZE) << 1) - 1; i ++){                                       // input=2048, window-size=256, stride=128, result-size=64, window-number=15
        audio_fft(input_buffer + FFT_RESULT_SIZE * i, dataRecorded + (FFT_WINDOW_SIZE >> 1) * i, FFT_WINDOW_SIZE);
    }
}


//******************************************************************************
// DMA interrupt service routine
// every time when the ADC is ready, ADC will trigger the DMA which starts
// transferring data from ADC to memory
//******************************************************************************
#pragma vector=DMA_VECTOR
__interrupt void dmaIsrHandler(void)
{
    switch (__even_in_range(DMAIV, DMAIV_DMA2IFG))
    {
        case DMAIV_DMA0IFG:
            __no_operation();
        case DMAIV_DMA1IFG:
            /* shut down audio collect module */
//            Audio_shutdownCollect();
            // Start Cpu on exit
            __bic_SR_register_on_exit(LPM3_bits);
            fft_interrupt();
            break;
        default: break;
   }
}


/* ADC12 Interrupt Service Routine*/
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
  switch(__even_in_range(ADC12IV,76))
  {
    case ADC12IV_NONE: break;                // Vector  0:  No interrupt
    case ADC12IV_ADC12OVIFG: break;          // Vector  2:  ADC12MEMx Overflow
    case ADC12IV_ADC12TOVIFG: break;         // Vector  4:  Conversion time overflow
    case ADC12IV_ADC12HIIFG: break;          // Vector  6:  ADC12HI
    case ADC12IV_ADC12LOIFG: break;          // Vector  8:  ADC12LO
    case ADC12IV_ADC12INIFG: break;           // Vector 10:  ADC12IN
    case ADC12IV_ADC12IFG0: break;
    case ADC12IV_ADC12IFG1: break;            // Vector 14:  ADC12MEM1
    case ADC12IV_ADC12IFG2: break;            // Vector 16:  ADC12MEM2
    case ADC12IV_ADC12IFG3: break;            // Vector 18:  ADC12MEM3
    case ADC12IV_ADC12IFG4: break;            // Vector 20:  ADC12MEM4
    case ADC12IV_ADC12IFG5: break;            // Vector 22:  ADC12MEM5
    case ADC12IV_ADC12IFG6: break;            // Vector 24:  ADC12MEM6
    case ADC12IV_ADC12IFG7: break;            // Vector 26:  ADC12MEM7
    case ADC12IV_ADC12IFG8: break;            // Vector 28:  ADC12MEM8
    case ADC12IV_ADC12IFG9: break;            // Vector 30:  ADC12MEM9
    case ADC12IV_ADC12IFG10: break;           // Vector 32:  ADC12MEM10
    case ADC12IV_ADC12IFG11: break;           // Vector 34:  ADC12MEM11
    case ADC12IV_ADC12IFG12: break;           // Vector 36:  ADC12MEM12
    case ADC12IV_ADC12IFG13: break;           // Vector 38:  ADC12MEM13
    case ADC12IV_ADC12IFG14: break;           // Vector 40:  ADC12MEM14
    case ADC12IV_ADC12IFG15: break;           // Vector 42:  ADC12MEM15
    case ADC12IV_ADC12IFG16: break;           // Vector 44:  ADC12MEM16
    case ADC12IV_ADC12IFG17: break;           // Vector 46:  ADC12MEM17
    case ADC12IV_ADC12IFG18: break;           // Vector 48:  ADC12MEM18
    case ADC12IV_ADC12IFG19: break;           // Vector 50:  ADC12MEM19
    case ADC12IV_ADC12IFG20: break;           // Vector 52:  ADC12MEM20
    case ADC12IV_ADC12IFG21: break;           // Vector 54:  ADC12MEM21
    case ADC12IV_ADC12IFG22: break;           // Vector 56:  ADC12MEM22
    case ADC12IV_ADC12IFG23: break;           // Vector 58:  ADC12MEM23
    case ADC12IV_ADC12IFG24: break;           // Vector 60:  ADC12MEM24
    case ADC12IV_ADC12IFG25: break;           // Vector 62:  ADC12MEM25
    case ADC12IV_ADC12IFG26: break;           // Vector 64:  ADC12MEM26
    case ADC12IV_ADC12IFG27: break;           // Vector 66:  ADC12MEM27
    case ADC12IV_ADC12IFG28: break;           // Vector 68:  ADC12MEM28
    case ADC12IV_ADC12IFG29: break;           // Vector 70:  ADC12MEM29
    case ADC12IV_ADC12IFG30: break;           // Vector 72:  ADC12MEM30
    case ADC12IV_ADC12IFG31: break;           // Vector 74:  ADC12MEM31
    case ADC12IV_ADC12RDYIFG: break;          // Vector 76:  ADC12RDY
    default: break;
  }
}
