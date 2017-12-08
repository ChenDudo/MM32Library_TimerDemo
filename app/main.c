////////////////////////////////////////////////////////////////////////////////
#define	_MAIN_C_
////////////////////////////////////////////////////////////////////////////////
#include "string.h"
#include "HAL_device.h"
#include "type.h"

#include "main.h"
#include "tim.h"
#include "tim3.h"
#include "adc.h"
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
    if(gCC <= TIMCCR) gCC++;
    if((gCC == TIMCCR) && (gBLDCState == 1)) 
       gBLDCState = 2;                                                          //BLDC Start is OK --> HOLD
}

////////////////////////////////////////////////////////////////////////////////
void BLDCStart()
{
    if(gCC >= TIMCCR) gCC = 0;
    BLDCSetCC(gCC, gCC, gCC);
}

////////////////////////////////////////////////////////////////////////////////
//int main(void)
//{
//    SystemInit();
//    InitSystick();
//    initRCC();
//    initTIM3(0, 4799);
//    initHallSensorTIM2(); 
//    initMotorBridgeTIM1(TIMPSC, TIMARR, HH);
//    initADC();
//    TIM1ON();
//    TIM2ON();
//    gBLDCState = 1;
//    gStep = 1;
//    gSimStep = 1;
//    while(1)
//    {
//        switch(gBLDCState)
//        {
//            case 0:
//            TIM1OFF();
//            break;
//            case 1:
//            BLDCStart();
//            break;
//            case 2:
//            //BLDCHoldOn(TIMCCR1, TIMCCR2, TIMCCR3);
//            BLDCHoldOn();
//            break;
//            case 3:
//            BLDCBreakEnable();
//            break;
//      }
//    }
//}
/*      the STEP table is suit for UM_MM32L3xx_v1.4 (figure.77)
--------------------------------------------------------------------------------
             | Step1 | Step2 | Step3 | Step4 | Step5 | Step6 |
   ----------------------------------------------------------
  |Channel1  |   1   |   1   |   0   |   0   |   0   |   0   |  GPIOA -- pin 8
   ----------------------------------------------------------  
  |Channel1N |   0   |   0   |   0   |   1   |   1   |   0   |  GPIOB -- pin 13
   ----------------------------------------------------------
  |Channel2  |   0   |   0   |   1   |   1   |   0   |   0   |  GPIOA -- pin 9
   ----------------------------------------------------------
  |Channel2N |   1   |   0   |   0   |   0   |   0   |   1   |  GPIOB -- pin 14
   ----------------------------------------------------------
  |Channel3  |   0   |   0   |   0   |   0   |   1   |   1   |	GPIOA -- pin 10
   ----------------------------------------------------------
  |Channel3N |   0   |   1   |   1   |   0   |   0   |   0   |	GPIOB -- pin 15
--------------------------------------------------------------------------------
  BKIN as the brake signal input pin to trigger the TIM1 source --> PB_12
--------------------------------------------------------------------------------
*/
int main(void)
{
    SystemInit();
    InitSystick();
    initRCC();
    //initTIM3(0, 4799);
    //initTIM3(0, 1199);
    initHallSensorTIM2(); 
    initMotorBridgeTIM1(TIMPSC, TIMARR, HH);
    //initADC();
    TIM1ON();
    TIM2ON();
    //TIM3ON();
    gBLDCState = 1;
    gStep = 1;
    gSimStep = 1;
    while(1)
    {
        switch(gBLDCState)
        {
            case 0:
            TIM1OFF();
            break;
            case 1:
            BLDCStart();
            break;
            case 2:
            //BLDCHoldOn(TIMCCR1, TIMCCR2, TIMCCR3);
            BLDCHoldOn();
            break;
            case 3:
            BLDCBreakEnable();
            break;
      }
    }
}