////////////////////////////////////////////////////////////////////////////////
#define _ADC_C_
////////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "HAL_conf.h"
#include "adc.h"

////////////////////////////////////////////////////////////////////////////////
void initADC()
{
    ADC_InitTypeDef  ADC_InitStructure;
        
    // input the GPIO of BLDC channel
    GPIO_InitTypeDef GPIO_InitStructure; 
    GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_4 | GPIO_Pin_5;                    //Miniboard Ain2 & Ain3
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    ADC_InitStructure.ADC_PRESCARE = ADC_PCLK2_PRESCARE_8;                      // Initialize the ADC_PRESCARE values
    ADC_InitStructure.ADC_Mode = ADC_Mode_Single_Period;                        // Initialize the ADC_Mode member
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                         // Initialize the ADC_ContinuousConvMode member
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                      // Initialize the ADC_DataAlign member
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC4;       // TIM1_CC4      
    ADC_Init(ADC2, &ADC_InitStructure);
    
    ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 0, 0);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_3, 0, 0);
    
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void ADCON()
{
    ADC_Cmd(ADC2, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void ADCOFF()
{
    ADC_Cmd(ADC2, DISABLE);
}
