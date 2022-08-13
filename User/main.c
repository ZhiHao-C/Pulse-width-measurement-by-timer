#include "stm32f10x.h"                  // Device header
#include "bps_led.h"
#include "bps_usart.h"
#include "bps_GeneralTim.h"
#include <stdio.h>

extern TIM_ICUserValueTypeDef TIM_ICUserValueStructure;



int main()
{
	
	float time=0;
	
	// TIM 计数器的驱动时钟
	uint32_t TIM_PscCLK = 72000000 / (GENERAL_TIM_PSC+1);
	
	/* 串口初始化 */
	USART_Config();
	
	/* 定时器初始化 */
	General_TIM_Init();
	
	printf ( "\r\n野火 STM32 输入捕获实验\r\n" );
	printf ( "\r\n按下K1，测试K1按下的时间\r\n" );
	
	while ( 1 )
	{
		if(TIM_ICUserValueStructure.Capture_FinishFlag == 1)
		{
			// 计算高电平时间的计数器的值
			time = (TIM_ICUserValueStructure.Capture_Period * (GENERAL_TIM_PERIOD+1)+ 
			       (TIM_ICUserValueStructure.Capture_CcrValue+1));
			
			// 打印高电平脉宽时间
			printf ( "\r\n测得高电平脉宽时间：%f s\r\n",time/TIM_PscCLK);
			
			TIM_ICUserValueStructure.Capture_FinishFlag = 0;			
		}		
	}
}

void TIM5_IRQHandler(void)
{
	
	//判断是否为更新中断
	if(TIM_GetITStatus(GENERAL_TIM,TIM_IT_Update)==1)
	{
		//
		TIM_ICUserValueStructure.Capture_Period++;
		//清除更新中断标志
		TIM_ClearITPendingBit(GENERAL_TIM,TIM_IT_Update);
	}
	//判断是否为捕获中断
	if(TIM_GetITStatus(GENERAL_TIM,TIM_IT_CC1)==1)
	{
		//第一次上升沿捕获
		if(TIM_ICUserValueStructure.Capture_StartFlag==0)
		{
			TIM_OC1PolarityConfig(GENERAL_TIM,TIM_OCPolarity_Low); //改变捕获触发极性
			TIM_ICUserValueStructure.Capture_CcrValue=0;         //捕获的值清零
			TIM_ICUserValueStructure.Capture_Period=0;           //
			TIM_ICUserValueStructure.Capture_FinishFlag=0;       //完成标志清零
			TIM_ICUserValueStructure.Capture_StartFlag=1;        //开始标志置一
			TIM_SetCounter(GENERAL_TIM,0);                        //清除Counter的值
			
		}
		//第二次下降沿捕获
		else
		{
			TIM_OC1PolarityConfig(GENERAL_TIM,TIM_OCPolarity_High); //改变捕获触发极性
			TIM_ICUserValueStructure.Capture_CcrValue=TIM_GetCapture1(GENERAL_TIM);//获取捕获到的count值
			TIM_ICUserValueStructure.Capture_FinishFlag=1;                         //结束标志置一
			TIM_ICUserValueStructure.Capture_StartFlag=0;                          //开始标志清零
		}
		TIM_ClearITPendingBit(GENERAL_TIM,TIM_IT_CC1);                           //清楚捕获中断标志
	}
}


