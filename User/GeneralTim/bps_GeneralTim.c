#include "bps_GeneralTim.h"

// ��ʱ�����벶���û��Զ�������ṹ�嶨��
TIM_ICUserValueTypeDef TIM_ICUserValueStructure = {0,0,0,0};


static void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;//���ñ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//����Ϊ��1�������ȼ�Ϊ1bit �����ȼ�Ϊ3bit��
	//�����Ǹ����жϻ��ǲ����ж϶�������TIM5_IRQn��
	NVIC_InitStruct.NVIC_IRQChannel=GENERAL_TIM_IRQ;//�����ж���Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;             //��
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}



static void General_TIM_GPIO_Config(void) 
{
	
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH1_GPIO_CLK, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=GENERAL_TIM_CH1_PIN;
	GPIO_Init(GENERAL_TIM_CH1_PORT, &GPIO_InitStructure);	
}



///*
// * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
// * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
// * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            ����
// *	TIM_CounterMode			     TIMx,x[6,7]û�У���������
// *  TIM_Period               ����
// *  TIM_ClockDivision        TIMx,x[6,7]û�У���������
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]����
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM�ź� ���ں�ռ�ձȵļ���--------------- */
// ARR ���Զ���װ�ؼĴ�����ֵ
// CLK_cnt����������ʱ�ӣ����� Fck_int / (psc+1) = 72M/(psc+1)
// PWM �źŵ����� T = ARR * (1/CLK_cnt) = ARR*(PSC+1) / 72M
// ռ�ձ�P=CCR/(ARR+1)
static void General_TIM_Mode_Config(void)
{
	RCC_APB1PeriphClockCmd(GENERAL_TIM_CLK,ENABLE);
	
/*--------------------ʱ���ṹ���ʼ��-------------------------*/	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM_TimeBaseStructure.TIM_Prescaler=GENERAL_TIM_PSC;
	
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_PERIOD;
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
	
/*--------------------���벶��ṹ���ʼ��-------------------*/	
	TIM_ICInitTypeDef TIM_ICInitStructure;
	// �������벶���ͨ������Ҫ���ݾ����GPIO������
	TIM_ICInitStructure.TIM_Channel=GENERAL_TIM_CHANNEL_x;
	// ���벶���źŵļ�������
	TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;
	// ����ͨ���Ͳ���ͨ����ӳ���ϵ����ֱ���ͷ�ֱ������
	TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;
	// �������Ҫ��������źŵķ�Ƶϵ��
	TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;
	// �������Ҫ��������źŵ��˲�ϵ��
	//�����˲�����ֻҪ�ɼ������ؾʹ�������
	TIM_ICInitStructure.TIM_ICFilter=0;
	
	TIM_ICInit(GENERAL_TIM, &TIM_ICInitStructure);
	
	
	// ���TIMx�Ĺ����־��
  TIM_ClearFlag(GENERAL_TIM, TIM_FLAG_Update|TIM_FLAG_CC1);	                //��
  // �������ºͲ����ж�  
	TIM_ITConfig (GENERAL_TIM, TIM_IT_Update | GENERAL_TIM_IT_CCx, ENABLE );
	
	// ʹ�ܼ�����
	TIM_Cmd(GENERAL_TIM, ENABLE);
}

void General_TIM_Init(void)
{
	NVIC_Config();
	General_TIM_GPIO_Config();
	General_TIM_Mode_Config();
}