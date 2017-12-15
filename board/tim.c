////////////////////////////////////////////////////////////////////////////////
#define _TIM_C_
////////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "HAL_conf.h"
#include "tim.h"

////////////////////////////////////////////////////////////////////////////////
void initRCC(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA |       
        RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC2, ENABLE);                    //enable APB2 PeriphClock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);  //enable APB1 PeriphClock
}

/*      the STEP table is suit for UM_MM32L3xx_v1.4 (figure.77)
--------------------------------------------------------------------------------
             | Step1 | Step2 | Step3 | Step4 | Step5 | Step6 |
   ----------------------------------------------------------
  |Channel1  |   1   |   1   |   0   |   0   |   0   |   0   |  GPIOA -- pin 8
   ------------------ ---------------------------------------- 
  |Channel1N |   0   |   0   |   0   |   1   |   1   |   0   |  GPIOB -- pin 13
   ----------------------------------------------------------
  |Channel2  |   0   |   0   |   0   |   0   |   1   |   1   |  GPIOA -- pin 9
   ----------------------------------------------------------
  |Channel2N |   0   |   1   |   1   |   0   |   0   |   0   |  GPIOB -- pin 14
   ----------------------------------------------------------
  |Channel3  |   0   |   0   |   1   |   1   |   0   |   0   |	GPIOA -- pin 10
   ----------------------------------------------------------
  |Channel3N |   1   |   0   |   0   |   0   |   0   |   1   |	GPIOB -- pin 15
--------------------------------------------------------------------------------
  BKIN as the brake signal input pin to trigger the TIM1 source --> PB_12
--------------------------------------------------------------------------------
*/
////////////////////////////////////////////////////////////////////////////////
void initMotorBridgeTIM1(u16 psc, u16 arr, u8 OCState)
{
    TIM_TimeBaseInitTypeDef TIM_StructInit;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_BDTRInitTypeDef  TIM_BDTRInitStructure; 
    GPIO_InitTypeDef GPIO_InitStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;                                      // if open IRQH, please undo
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;        // CH1-3 output: PA_8 | PA_9 | PA_10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                             // Multiplex push output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                           // speed 50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                      // init GPIOA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;      // CH1-3N output: PA_13 | PA_14 | PA_15
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                      // init GPIOB
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                                  // Break input: PB_12
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                       // Floating input
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                      // init GPIOB
    
    TIM_StructInit.TIM_Prescaler = psc;                                         // set Prescaler: psc + 1
    TIM_StructInit.TIM_Period = arr;                                            // set Period: arr + 1
    TIM_StructInit.TIM_ClockDivision = TIM_CKD_DIV1;                            // set ClockDivision :0                   
    TIM_StructInit.TIM_CounterMode = TIM_CounterMode_CenterAligned2;
    TIM_StructInit.TIM_RepetitionCounter = 0;                                   
    TIM_TimeBaseInit(TIM1, &TIM_StructInit);
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;                         // Channel 1, 2, 3 ¨C set to PWM mode - all 6 outputs           
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = TIMCCR;                                     // BLDC_ccr_val
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    switch(OCState)
    {
        case 0:
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
        TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
        break;
        case 1:
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
        TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
        break;
        case 2:
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
        TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
        break;
        default:
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
        TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
        break;
    }
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                           // The CC4 channel to enable ADC
    TIM_OCInitStructure.TIM_Pulse = 3;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);
    
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);                           // activate preloading the CCR register
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
    
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStructure.TIM_DeadTime = DEADTIME;                              // see programmers reference manual
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;                        // diable the Break         
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;        
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
    
    TIM_CCPreloadControl(TIM1, ENABLE);                                         // preload ARR register
    Hall2MotorON();                                                             // activate COM (Commutation) Event from Slave (HallSensor timer) through TRGI
    TIM_SelectInputTrigger(TIM1, TIM_TS_ITR1);                                  // Internal connection from Hall/Enc Timer to Motor Timer 
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    
    //TIM_ITConfig(TIM1, TIM_IT_COM, ENABLE);                                   // if open IRQH, please undo
    //NVIC_InitStructure.NVIC_IRQChannel = TIM1_TRG_COM_IRQn;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
//void TIM1_TRG_COM_IRQHandler(void)                                            // if open IRQH, please undo
//{ 
//    if (TIM_GetITStatus(TIM1, TIM_IT_COM) != RESET) {
//        TIM_ClearITPendingBit(TIM1, TIM_IT_COM);
//    }
//    //BLDCSetCC(TIMCCR, TIMCCR, TIMCCR);
//}

/*//////////////////////////////////////////////////////////////////////////////
   PA_0  : hall input channel1  
   PA_1  : hall input channel2  
   PA_2  : hall input channel3
//////////////////////////////////////////////////////////////////////////////*/
void initHallSensorTIM2(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;         // TIM2 hall input
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                           // PA_0 | PA_1 | PA_2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    TIM_DeInit(MOTOR_HALL_TIM);
    
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);   
    
    TIM_SelectHallSensor(TIM2, ENABLE);                                         // enable hall sensor
     
    TIM_SelectInputTrigger(TIM2, TIM_TS_TI1F_ED);                               // Signal TI1F_ED: falling and rising ddge of the inputs is used
 
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);                             // HallSensor event is delivered with singnal TI1F_ED On every TI1F_ED event the counter is resetted and update is tiggered 
      
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;                            // the timervalue is copied into ccr register and a CCR1 Interrupt
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_TRC;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x1;                                     // noise filter
    TIM_ICInit(TIM2, &TIM_ICInitStructure);                                     // TIM_IT_CC1 is fired 
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
    TIM_OCInitStructure.TIM_Pulse = 0x1;                                        // reduce delay
    TIM_OC2Init(TIM2,&TIM_OCInitStructure);
    
    TIM_CCPreloadControl(TIM2, ENABLE);                                         // Enable ARR preload 
    TIM_ClearFlag(TIM2, TIM_FLAG_CC2);
    
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_OC2Ref);                       // timer2 output compate signal is connected to TRIGO 
    
    TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);                        // Enable channel 2 compate interrupt request
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;                
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);                                             // Hall has highest priority
}

////////////////////////////////////////////////////////////////////////////////
//  TIM2_IRQHandler Note: after commutation the next motor step must be prepared
////////////////////////////////////////////////////////////////////////////////
void TIM2_IRQHandler(void)
{      
    if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET) {
        TIM_ClearITPendingBit(MOTOR_HALL_TIM, TIM_IT_CC2);
        hallccr1 = TIM2->CCR1;                                                  // calculate motor  speed or else with CCR1 values
        hallpos = (GPIO_ReadInputData(GPIOA)) & 0x0007;                         // get step position by PA_0 / PA_1 / PA_2
        BLDCMotorPrepareCommutation(hallpos, TIM_OCMode_PWM1, TIM_OCMode_Active); //can also choose (TIM_OCMode_PWM1, TIM_OCMode_PWM2)
    }
}

/* the TIH1/2/3 value is suit for UM_MM32L3xx_v1.4 (figure.77)
--------------------------------------------------------------------------------
 PA_0    |   _|- - -|_ _ _|- - -| _ _ _ _ _
         |
 PA_1    |   _ _|- - -|_ _ _|- - -| _ _ _ _
         |
 PA_2    |   _ _ _|- - -|_ _ _|- - -| _ _ _
---------|----------------------------------------------------------------------
pos value|   0 1 3 7 6 4 0 1 3 7 6 4 .....
--------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////*/
void BLDCMotorPrepareCommutation(u8 pos, u16 TIM_OCMode, u16 TIM_NOCMode)
{
    if (pos == 1){
        //Next step: Step 2 Configuration
        TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode);                        // Channel1 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
        
        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);                       // Channel2 configuration
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable); 
        
        TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_NOCMode);                       // Channel3 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);  
    }
    else if (pos == 3){
        // Next step: Step 3 Configuration
        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);                       // Channel1 configuration
        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
        
        TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode);                        // Channel2 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
        
        TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_NOCMode);                       // Channel3 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);
    }
    else if (pos == 7){
        // Next step: Step 4 Configuration
        TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_NOCMode);                       // Channel1 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable); 
        
        TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode);                        // Channel2 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);   
        
        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);                       // Channel3 configuration
        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
    }
    else if (pos == 6){
        // Next step: Step 5 Configuration
        TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_NOCMode);                       // Channel1 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
        
        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);                       // Channel2 configuration
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
        
        TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode);                        // Channel3 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
    }
    else if (pos == 4){
        // Next step: Step 6 Configuration
        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);                       // Channel1 configuration
        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
        
        TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_NOCMode);                       // Channel2 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
        
        TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode);                        // Channel3 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
    }
    else if (pos == 0){
        // Next step: Step 1 Configuration      
        TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode);                        // Channel1 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable); 
        
        TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_NOCMode);                       // Channel2 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
        
        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);                       // Channel3 configuration        
        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
    }
}

////////////////////////////////////////////////////////////////////////////////
void Hall2MotorON()
{
    TIM_SelectCOM(TIM1, ENABLE);                                                // enable the connection between HallTimer and MotorTimer
}

////////////////////////////////////////////////////////////////////////////////
void Hall2MotorOFF()
{
    TIM_SelectCOM(TIM1, DISABLE);                                               // disable the connection between HallTimer and MotorTimer
}

////////////////////////////////////////////////////////////////////////////////
void TIM1ON()                                                                   // enable motor timer
{
    TIM_Cmd(TIM1, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void TIM1OFF()                                                                  // disable motor timer
{
    TIM_Cmd(TIM1, DISABLE);
}

////////////////////////////////////////////////////////////////////////////////
void TIM2ON()
{
    TIM_Cmd(TIM2, ENABLE);                                                      // enable hall timer
}

////////////////////////////////////////////////////////////////////////////////
void TIM2OFF()
{
    TIM_Cmd(TIM2, DISABLE);                                                     // disable motor timer
}

////////////////////////////////////////////////////////////////////////////////
void BLDCSetCC(u16 CC1, u16 CC2, u16 CC3)
{
    TIM_SetCompare1(TIM1, CC1);                                                 // set TIM1->CCR1 value
    TIM_SetCompare2(TIM1, CC2);                                                 // set TIM1->CCR2 value
    TIM_SetCompare3(TIM1, CC3);                                                 // set TIM1->CCR3 value 
}

////////////////////////////////////////////////////////////////////////////////
void BLDCBreakEnable()
{
    TIM1->BDTR |= 1 << 12;                                                      // enale TIM1 Break
}