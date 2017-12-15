////////////////////////////////////////////////////////////////////////////////
#ifndef __TIM_H_
#define __TIM_H_
////////////////////////////////////////////////////////////////////////////////

#define MOTOR_OUTPUT_TIM        TIM1
#define MOTOR_HALL_TIM       TIM2

//#define TIM_OCMode_Active       0x0050
//#define TIM_OCMode_Inactive    0x0040
#define TIMPSC                  0
#define TIMARR                  25
#define TIMCCR                  12
//#define TIMARR                  255
//#define TIMCCR                  127
//#define TIMARR                  4095
//#define TIMCCR                  2047
#define TIMCCR1                 2047
#define TIMCCR2                 1023
#define TIMCCR3                 511
#define DEADTIME                0x00

typedef enum {
    LL,
    LH,
    HL,
    HH
} OCState;

////////////////////////////////////////////////////////////////////////////////
#ifdef _TIM_C_
#define GLOBAL
#else 
#define GLOBAL extern
#endif

GLOBAL u16 hallccr1;
GLOBAL u8 gStep;
GLOBAL u8 hallpos;

#undef GLOBAL
////////////////////////////////////////////////////////////////////////////////
void initRCC(void);
void initMotorBridgeTIM1(u16 psc, u16 arr, u8 OCState);
void initHallSensorTIM2();
void BLDCMotorPrepareCommutation(u8 pos, u16 TIM_OCMode, u16 TIM_NOCMode);
void Hall2MotorON();
void Hall2MotorOFF();
void BLDCBreakEnable();
void BLDCHoldOn();
void BLDCSetCC(u16 CC1, u16 CC2, u16 CC3);
void TIM1ON();
void TIM1OFF();
void TIM2ON();
////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////