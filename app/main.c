////////////////////////////////////////////////////////////////////////////////
#define	_MAIN_C_
////////////////////////////////////////////////////////////////////////////////
#include "string.h"
#include "HAL_device.h"
#include "type.h"

#include "main.h"
#include "tim.h"
#include "tim3.h"                                                               // if not use, please ignore
#include "adc.h"                                                                // if not use, please ignore

////////////////////////////////////////////////////////////////////////////////
void InitSystick()
{
    SysTick_Config(SystemCoreClock / 1000);
    NVIC_SetPriority(SysTick_IRQn, 0x00);
}

////////////////////////////////////////////////////////////////////////////////
void SysTick_Handler()
{
    static u32 cnt;
    cnt++;
}


/* /////////////////////////////////////////////////////////////////////////////
--------------------------------------------------------------------------------
      TIM2 (hall timer)            |    TIM3 (Simulate the hall output)
--------------------------------------------------------------------------------
     PA_0  : hall input channel1   |     PA_6 (TIM3_CH1)
--------------------------------------------------------------------------------
     PA_1  : hall input channel2   |     PA_7 (TIM3_CH2)
--------------------------------------------------------------------------------
     PA_2  : hall input channel3   |     PB_0 (TIM3_CH3)
--------------------------------------------------------------------------------
NOTE: if use TIM3 simulate, In order to achieve better results, please use anot-
her board to output and the main function just only init TIM3 and its parameter.

--------------------------------------------------------------------------------
                      TIM1 (motor output timer)
--------------------------------------------------------------------------------
     PA_8  : TIM1_CH1              |      PA_13   : TIM1_CH1N
--------------------------------------------------------------------------------
     PA_9  : TIM1_CH2              |      PA_14   : TIM1_CH2N
--------------------------------------------------------------------------------
     PA_10 : TIM1_CH3              |      PA_15   : TIM1_CH3N
--------------------------------------------------------------------------------
*///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    SystemInit();                                                               // init systemClock:96MHz
    InitSystick();                                                              // init systick
    initRCC();                                                                  // enable RCC
    
    //initTIM3(0, 1199);                                                        // init TIM3
    //TIM3ON();                                                                 // enable TIM3
    //gSimStep = 1;                                                             // used at TIM3_IRQH

    initHallSensorTIM2();                                                       // init TIM2
    initMotorBridgeTIM1(TIMPSC, TIMARR, HH);                                    // init TIM1(could choose LL/LH/HL/HH)
    
    TIM1ON();                                                                   // enable TIM1
    TIM2ON();                                                                   // enable TIM2

    //initADC();                                                                // NOTE: if use ADC, please init ADC and check ADC.C config 

    while(1){
        
    }
}