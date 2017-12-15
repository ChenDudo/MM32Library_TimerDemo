////////////////////////////////////////////////////////////////////////////////
#define _TIM3_C_
////////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "HAL_conf.h"
#include "tim3.h"

////////////////////////////////////////////////////////////////////////////////
//                              NOTICE
//      the TIM3 is just simulate the hallsensor 3' input signal
//      if not use, please delete tim3 part and config initRCC() 
////////////////////////////////////////////////////////////////////////////////
void initTIM3(u16 psc, u16 arr)
{
    TIM_TimeBaseInitTypeDef TIM_StructInit;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    TIM_DeInit(TIM3);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;                      // TIM3 CH1/2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                                                   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                                   // TIM3 CH3
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    TIM_StructInit.TIM_Prescaler = psc;                                         // set Prescaler: psc + 1
    TIM_StructInit.TIM_Period = arr;                                            // set Period: arr + 1
    TIM_StructInit.TIM_ClockDivision = TIM_CKD_DIV1;                            // set ClockDivision :0                         
    TIM_StructInit.TIM_CounterMode = TIM_CounterMode_Up;                                              
    TIM_TimeBaseInit(TIM3, &TIM_StructInit);   
    
    TIM_CCPreloadControl(TIM3, ENABLE);                                         // Enable ARR preload 
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                                  // Enable update interrupt
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM3, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void TIM3ON()
{
    TIM_Cmd(TIM3, ENABLE);                                                      // enable simulate timer
}

////////////////////////////////////////////////////////////////////////////////
void TIM3OFF()
{
    TIM_Cmd(TIM3, DISABLE);                                                     // disable simulate timer
}

////////////////////////////////////////////////////////////////////////////////
void TIM3_IRQHandler(void)
{      
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) { 
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
        simMotorPrepareCommutation();                                           //its similar to TIM2 commutation the next motor step prepared
    }
}

////////////////////////////////////////////////////////////////////////////////
void simMotorPrepareCommutation()
{
    if (gSimStep == 1){
        //Next step: Step 2 Configuration
        CH1ON;
        CH2ON;
        CH3OFF;
        gSimStep++;
    }
    else if (gSimStep == 2){
        // Next step: Step 3 Configuration
        CH1ON;
        CH2ON;
        CH3ON;
        gSimStep++;
    }
    else if (gSimStep == 3){
        // Next step: Step 4 Configuration
        CH1OFF;
        CH2ON;
        CH3ON;
        gSimStep++;
    }
    else if (gSimStep == 4){
        // Next step: Step 5 Configuration
        CH1OFF;
        CH2OFF;
        CH3ON;
        gSimStep++;
    }
    else if (gSimStep == 5){
        // Next step: Step 6 Configuration
        CH1OFF;
        CH2OFF;
        CH3OFF;
        gSimStep++;
    }
    else{
        // Next step: Step 1 Configuration      
        CH1ON;
        CH2OFF;
        CH3OFF;  
        gSimStep = 1;
    }   
}