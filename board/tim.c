////////////////////////////////////////////////////////////////////////////////
#define _TIM_C_
////////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "HAL_conf.h"
#include "tim.h"

////////////////////////////////////////////////////////////////////////////////
void initRCC(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA |\
        RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);
}

/*      the STEP table is suit for UM_MM32L3xx_v1.4 (figure.77)
--------------------------------------------------------------------------------
             | Step1 | Step2 | Step3 | Step4 | Step5 | Step6 |
   ----------------------------------------------------------
  |Channel1  |   1   |   0   |   0   |   0   |   0   |   1   |  GPIOA -- pin 8
   ----------------------------------------------------------  
  |Channel1N |   0   |   0   |   1   |   1   |   0   |   0   |  GPIOB -- pin 13
   ----------------------------------------------------------
  |Channel2  |   0   |   0   |   0   |   1   |   1   |   0   |  GPIOA -- pin 9
   ----------------------------------------------------------
  |Channel2N |   1   |   1   |   0   |   0   |   0   |   0   |  GPIOB -- pin 14
   ----------------------------------------------------------
  |Channel3  |   0   |   1   |   1   |   0   |   0   |   0   |	GPIOA -- pin 10
   ----------------------------------------------------------
  |Channel3N |   0   |   0   |   0   |   0   |   1   |   1   |	GPIOB -- pin 15
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
    NVIC_InitTypeDef NVIC_InitStructure;
    
    TIM_DeInit(TIM1);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;        //CH1-3 output: PA_8 | PA_9 | PA_10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;      // CH1-3N output: PA_13 | PA_14 | PA_15
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                                  // Break input: PB_12
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
    
    TIM_StructInit.TIM_Prescaler = psc;
    TIM_StructInit.TIM_Period = arr;
    TIM_StructInit.TIM_ClockDivision = TIM_CKD_DIV1;                                                  
    TIM_StructInit.TIM_CounterMode = TIM_CounterMode_CenterAligned2;                                              
    TIM_StructInit.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_StructInit);
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = TIMCCR; 
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;      //??
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
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_Pulse = TIMCCR;                                         // The CC4 channel to enable ADC
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);
    
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
    
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;             
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;             
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;                
    TIM_BDTRInitStructure.TIM_DeadTime = DEADTIME;                               
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;                        // diable the Break         
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;        
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
    
    TIM_CCPreloadControl(TIM1, ENABLE);
    Hall2MotorON();   
    // Internal connection from Hall/Enc Timer to Motor Timer// TIM2 COM -> TIM1
    TIM_SelectInputTrigger(TIM1, TIM_TS_ITR1);
    
    //TIM_ITConfig(TIM1, TIM_IT_COM, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_TRG_COM_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    //TIM_Cmd(TIM1, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void TIM1_TRG_COM_IRQHandler(void)
{ 
    if (TIM_GetITStatus(TIM1, TIM_IT_COM) != RESET) {
        TIM_ClearITPendingBit(TIM1, TIM_IT_COM);
        // commutationCount++;
    }
    //....
    //BLDCSetCC(TIMCCR, TIMCCR, TIMCCR);
}

////////////////////////////////////////////////////////////////////////////////
void initHallSensorTIM2(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;         // TIM2 hall input
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                           // PA_1 | PA_2 | PA_3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    TIM_DeInit(MOTOR_HALLENC_TIM);
    
    //TIM_TimeBaseStructure.TIM_Prescaler = 95;
    TIM_TimeBaseStructure.TIM_Prescaler = 1;
    //TIM_TimeBaseStructure.TIM_Prescaler = 126; 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);   
    
    TIM_SelectHallSensor(TIM2, ENABLE);// enable hall sensor
    // Signal TI1F_ED: falling and rising ddge of the inputs is used 
    TIM_SelectInputTrigger(TIM2, TIM_TS_TI1F_ED);// HallSensor event is delivered with singnal TI1F_ED
    // On every TI1F_ED event the counter is resetted and update is tiggered 
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);
    
    // on every TCR edge (build from TI1F_ED which is a HallSensor edge)  
    // the timervalue is copied into ccr register and a CCR1 Interrupt
    // TIM_IT_CC1 is fired 
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_TRC;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    // noise filter: 1111 => 72000kHz / factor (==1) / 32 / 8 -> 281kHz
    //TIM_ICInitStructure.TIM_ICFilter = 0xF;
    TIM_ICInitStructure.TIM_ICFilter = 0x1;
    TIM_ICInit(TIM2, &TIM_ICInitStructure);
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
    TIM_OCInitStructure.TIM_Pulse = 0x1;                                        // reduce delay
    TIM_OC2Init(TIM2,&TIM_OCInitStructure);
    
    TIM_CCPreloadControl(TIM2, ENABLE);
    TIM_ClearFlag(TIM2, TIM_FLAG_CC2);
    
    //TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_OC2Ref);// timer2 output compate signal is connected to TRIGO 
    
    TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
    
    //TIM_Cmd(TIM2, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
//      deal with the motor(TIM1) commutation operation
////////////////////////////////////////////////////////////////////////////////
void TIM2_IRQHandler(void)
{      
//    if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) { 
//        TIM_ClearITPendingBit(MOTOR_HALLENC_TIM, TIM_IT_CC1);  
//        hallccr1 = TIM2->CCR1;                                                  // calculate motor speed or else with CCR1 values
//        hallpos = (GPIO_ReadInputData(GPIOA)) & 0x0007;
//        
////            gStep = 6;
////        return;
//        //else gStep = 0;
//
////...
//    }
//    else if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET) {
    if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET) {
        TIM_ClearITPendingBit(MOTOR_HALLENC_TIM, TIM_IT_CC2);
        hallccr1 = TIM2->CCR1; 
        hallpos = (GPIO_ReadInputData(GPIOA)) & 0x0007;
        BLDCMotorPrepareCommutation(hallpos, TIM_OCMode_Active, TIM_OCMode_Active); 
        //BLDCMotorPrepareCommutation(TIM_OCMode_PWM1, TIM_OCMode_PWM2); 
    }
//    else { 
//        ;                                                                       // this should not happen 
//    } 
}

////////////////////////////////////////////////////////////////////////////////
void BLDCMotorPrepareCommutation(u8 pos, u16 TIM_OCMode, u16 TIM_NOCMode)
{
    if (pos == 1){
        //Next step: Step 2 Configuration
        TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode);                   // Channel1 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
        
        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);                       // Channel2 configuration
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable); 
        
        TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_NOCMode);                            // Channel3 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);  
                   
        //gStep++;
    }
    else if (pos == 3){
        // Next step: Step 3 Configuration
        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);                       // Channel1 configuration
        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
        
        TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode);                   // Channel2 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
        
        TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_NOCMode);                            // Channel3 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);
        
        //gStep++;
    }
    else if (pos == 7){
        // Next step: Step 4 Configuration
        TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_NOCMode);                            // Channel1 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable); 
        
        TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode);                   // Channel2 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);   
        
        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);                       // Channel3 configuration
        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
        //gStep++;
    }
    else if (pos == 6){
        // Next step: Step 5 Configuration
        TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_NOCMode);                            // Channel1 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
        
        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);                       // Channel2 configuration
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
        
        TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode);                   // Channel3 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
        
        //gStep++;
    }
    else if (pos == 4){
        // Next step: Step 6 Configuration
        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);                       // Channel1 configuration
        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
        
        TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_NOCMode);                            // Channel2 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
        
        TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode);                   // Channel3 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
        
        //gStep++;
    }
    else if (pos == 0){
        // Next step: Step 1 Configuration      
        TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode);                   // Channel1 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable); 
        
        TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_NOCMode);                            // Channel2 configuration
        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
        
        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);                       // Channel3 configuration        
        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
        
        //gStep = 1;
    }
}
//void BLDCMotorPrepareCommutation(u16 TIM_OCMode, u16 TIM_NOCMode)
//{
//    if (gStep == 1){
//        //Next step: Step 2 Configuration
//        TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode);                   // Channel1 configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
//        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
//        
//        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);                       // Channel2 configuration
//        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable); 
//        
//        TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_NOCMode);                            // Channel3 configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
//        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);  
//                   
//        gStep++;
//    }
//    else if (gStep == 2){
//        // Next step: Step 3 Configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);                       // Channel1 configuration
//        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
//        
//        TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode);                   // Channel2 configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
//        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
//        
//        TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_NOCMode);                            // Channel3 configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
//        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);
//        
//        gStep++;
//    }
//    else if (gStep == 3){
//        // Next step: Step 4 Configuration
//        TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_NOCMode);                            // Channel1 configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
//        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable); 
//        
//        TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode);                   // Channel2 configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
//        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);   
//        
//        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);                       // Channel3 configuration
//        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
//        gStep++;
//    }
//    else if (gStep == 4){
//        // Next step: Step 5 Configuration
//        TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_NOCMode);                            // Channel1 configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
//        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
//        
//        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);                       // Channel2 configuration
//        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
//        
//        TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode);                   // Channel3 configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
//        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
//        
//        gStep++;
//    }
//    else if (gStep == 5){
//        // Next step: Step 6 Configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);                       // Channel1 configuration
//        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
//        
//        TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_NOCMode);                            // Channel2 configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
//        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
//        
//        TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode);                   // Channel3 configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
//        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
//        
//        gStep++;
//    }
//    else{
//        // Next step: Step 1 Configuration      
//        TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode);                   // Channel1 configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
//        TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable); 
//        
//        TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_NOCMode);                            // Channel2 configuration
//        TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
//        TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
//        
//        TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);                       // Channel3 configuration        
//        TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
//        
//        gStep = 1;
//    }
//}

////////////////////////////////////////////////////////////////////////////////
void Hall2MotorON()
{
    TIM_SelectCOM(TIM1, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void Hall2MotorOFF()
{
    TIM_SelectCOM(TIM1, DISABLE);
}

////////////////////////////////////////////////////////////////////////////////
void TIM1ON()
{
    TIM_Cmd(TIM1, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void TIM1OFF()
{
    TIM_Cmd(TIM1, DISABLE);
}

////////////////////////////////////////////////////////////////////////////////
void TIM2ON()
{
    TIM_Cmd(TIM2, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void TIM2OFF()
{
    TIM_Cmd(TIM2, DISABLE);
}

////////////////////////////////////////////////////////////////////////////////
void BLDCHoldOn()
{

}

////////////////////////////////////////////////////////////////////////////////
void BLDCSetCC(u16 CC1, u16 CC2, u16 CC3)
{
    TIM_SetCompare1(TIM1, CC1);
    TIM_SetCompare2(TIM1, CC2);
    TIM_SetCompare3(TIM1, CC3); 
}

////////////////////////////////////////////////////////////////////////////////
void BLDCBreakEnable()
{
    TIM1->BDTR |= 1 << 12;                                                      // enale TIM1 Break
}