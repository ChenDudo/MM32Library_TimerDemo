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
    //TIM_OCInitTypeDef  TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    TIM_DeInit(TIM3);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;                      //TIM3 CH1/2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                                                   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                                   //TIM3 CH3
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    TIM_StructInit.TIM_Prescaler = psc;
    TIM_StructInit.TIM_Period = arr;
    TIM_StructInit.TIM_ClockDivision = TIM_CKD_DIV1;                                                  
    TIM_StructInit.TIM_CounterMode = TIM_CounterMode_Up;                                              
    TIM_TimeBaseInit(TIM3, &TIM_StructInit);
    
    //    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
    //    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    //    TIM_OCInitStructure.TIM_Pulse = 0x2222; 
    //    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    //    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    //    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    //    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    //    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    //    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    //    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    //    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    //    TIM_CtrlPWMOutputs(TIM3, ENABLE);
    
    TIM_CCPreloadControl(TIM3, ENABLE);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
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
    TIM_Cmd(TIM3, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void TIM3OFF()
{
    TIM_Cmd(TIM3, DISABLE);
}

////////////////////////////////////////////////////////////////////////////////
void TIM3_IRQHandler(void)
{      
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) { 
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
        simMotorPrepareCommutation(); 
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

////////////////////////////////////////////////////////////////////////////////
/*
void simMotorPrepareCommutation()
{
if (gSimStep == 1){
//Next step: Step 2 Configuration
TIM_SelectOCxM(TIM3, TIM_Channel_1, 0x0050);                   // Channel1 configuration
TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Enable);

TIM_SelectOCxM(TIM3, TIM_Channel_2, 0x0050); 
TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);

TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Disable);                       // Channel2 configuration

gSimStep++;
    }
    else if (gSimStep == 2){
// Next step: Step 3 Configuration
TIM_SelectOCxM(TIM3, TIM_Channel_1, 0x0050);                   // Channel1 configuration
TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Enable);

TIM_SelectOCxM(TIM3, TIM_Channel_2, 0x0050); 
TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);

TIM_SelectOCxM(TIM3, TIM_Channel_3, 0x0050);
TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Enable); 
gSimStep++;
    }
    else if (gSimStep == 3){
// Next step: Step 4 Configuration
// Channel1 configuration
TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Disable);

TIM_SelectOCxM(TIM3, TIM_Channel_2, 0x0050); 
TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);

TIM_SelectOCxM(TIM3, TIM_Channel_3, 0x0050);
TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Enable);  
gSimStep++;
    }
    else if (gSimStep == 4){
// Next step: Step 5 Configuration
TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Disable);

TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);

TIM_SelectOCxM(TIM3, TIM_Channel_3, 0x0050);
TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Enable);
gSimStep++;
    }
    else if (gSimStep == 5){
// Next step: Step 6 Configuration
TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Disable);

TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);

TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Disable);
gSimStep++;
    }
    else{
// Next step: Step 1 Configuration      
TIM_SelectOCxM(TIM3, TIM_Channel_1, 0x0050);                   // Channel1 configuration
TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Enable);

TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);

TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Disable);  
gSimStep = 1;
    }

}*/