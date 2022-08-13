#include "stm32f10x.h"                  // Device header
#include "bps_led.h"
#include "bps_usart.h"
#include "bps_GeneralTim.h"
#include <stdio.h>

extern TIM_ICUserValueTypeDef TIM_ICUserValueStructure;



int main()
{
	
	float time=0;
	
	// TIM ������������ʱ��
	uint32_t TIM_PscCLK = 72000000 / (GENERAL_TIM_PSC+1);
	
	/* ���ڳ�ʼ�� */
	USART_Config();
	
	/* ��ʱ����ʼ�� */
	General_TIM_Init();
	
	printf ( "\r\nҰ�� STM32 ���벶��ʵ��\r\n" );
	printf ( "\r\n����K1������K1���µ�ʱ��\r\n" );
	
	while ( 1 )
	{
		if(TIM_ICUserValueStructure.Capture_FinishFlag == 1)
		{
			// ����ߵ�ƽʱ��ļ�������ֵ
			time = (TIM_ICUserValueStructure.Capture_Period * (GENERAL_TIM_PERIOD+1)+ 
			       (TIM_ICUserValueStructure.Capture_CcrValue+1));
			
			// ��ӡ�ߵ�ƽ����ʱ��
			printf ( "\r\n��øߵ�ƽ����ʱ�䣺%f s\r\n",time/TIM_PscCLK);
			
			TIM_ICUserValueStructure.Capture_FinishFlag = 0;			
		}		
	}
}

void TIM5_IRQHandler(void)
{
	
	//�ж��Ƿ�Ϊ�����ж�
	if(TIM_GetITStatus(GENERAL_TIM,TIM_IT_Update)==1)
	{
		//
		TIM_ICUserValueStructure.Capture_Period++;
		//��������жϱ�־
		TIM_ClearITPendingBit(GENERAL_TIM,TIM_IT_Update);
	}
	//�ж��Ƿ�Ϊ�����ж�
	if(TIM_GetITStatus(GENERAL_TIM,TIM_IT_CC1)==1)
	{
		//��һ�������ز���
		if(TIM_ICUserValueStructure.Capture_StartFlag==0)
		{
			TIM_OC1PolarityConfig(GENERAL_TIM,TIM_OCPolarity_Low); //�ı䲶�񴥷�����
			TIM_ICUserValueStructure.Capture_CcrValue=0;         //�����ֵ����
			TIM_ICUserValueStructure.Capture_Period=0;           //
			TIM_ICUserValueStructure.Capture_FinishFlag=0;       //��ɱ�־����
			TIM_ICUserValueStructure.Capture_StartFlag=1;        //��ʼ��־��һ
			TIM_SetCounter(GENERAL_TIM,0);                        //���Counter��ֵ
			
		}
		//�ڶ����½��ز���
		else
		{
			TIM_OC1PolarityConfig(GENERAL_TIM,TIM_OCPolarity_High); //�ı䲶�񴥷�����
			TIM_ICUserValueStructure.Capture_CcrValue=TIM_GetCapture1(GENERAL_TIM);//��ȡ���񵽵�countֵ
			TIM_ICUserValueStructure.Capture_FinishFlag=1;                         //������־��һ
			TIM_ICUserValueStructure.Capture_StartFlag=0;                          //��ʼ��־����
		}
		TIM_ClearITPendingBit(GENERAL_TIM,TIM_IT_CC1);                           //��������жϱ�־
	}
}


