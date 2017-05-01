

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
	  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define  fac_us        (9)		   
//#define  fac_ms        (9000)
static int fac_us;
static int fac_ms;


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if SYSTEM_SUPPORT_UCOS
void SysTick_Handler(void)
{				   
	OSIntEnter();		    //进入中断
	OSTimeTick();       //调用ucos的时钟服务程序               
	OSIntExit();        //触发任务切换软中断
}
#endif

			   

/*******************************************************************************
* Function Name  : delay_init.
* Descriptioan   : 初始化延迟函数
*                  当使用ucos的时候,此函数会初始化ucos的时钟节拍
*                  SYSTICK的时钟固定为HCLK时钟的1/8
*                  SYSCLK:系统时钟
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void delay_init()
{
	//SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8  
	
#if SYSTEM_SUPPORT_UCOS 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	u32 reload;
#endif
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us=SystemCoreClock/8000000;	//为系统时钟的1/8  
	 
#if SYSTEM_SUPPORT_UCOS 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	reload=SystemCoreClock/8000000;		//每秒钟的计数次数 单位为K	   
	reload*=1000000/OS_TICKS_PER_SEC;//根据OS_TICKS_PER_SEC设定溢出时间
							//reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右	
	fac_ms=1000/OS_TICKS_PER_SEC;//代表ucos可以延时的最少单位	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 	//每1/OS_TICKS_PER_SEC秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK    
#else
	fac_ms=(u16)fac_us*1000;//非ucos下,代表每个ms需要的systick时钟数   
#endif
}								    



/*******************************************************************************
* Function Name  : delay_us.
* Descriptioan   : 微妙延时.
* Input          : 微妙.
* Output         : None.
* Return         : None.
*******************************************************************************/	
void delay_us(u32 nus)
{		
#if SYSTEM_SUPPORT_UCOS==0
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器
#else
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;	          //LOAD的值	    	 
	ticks=nus*fac_us; 			            //需要的节拍数	  		 
	tcnt=0;
	OSSchedLock();				              //阻止ucos调度，防止打断us延时
	told=SysTick->VAL;        	        //刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;   //这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;           //时间超过/等于要延迟的时间,则退出.
		}  
	};
	OSSchedUnlock();			              //开启ucos调度 
#endif		
}



/*******************************************************************************
* Function Name  : delay_ms.
* Descriptioan   : 毫秒延时.
* Input          : 毫秒.
* Output         : None.
* Return         : None.
*******************************************************************************/
void delay_ms(u16 nms)
{	
#if SYSTEM_SUPPORT_UCOS==0
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器 
#else
	if(OSRunning==TRUE)               //如果os已经在跑了	    
	{		  
		if(nms>=fac_ms)                 //延时的时间大于ucos的最少时间周期 
		{
   			OSTimeDly(nms/fac_ms);      //ucos延时
		}
		nms%=fac_ms;			              //ucos已经无法提供这么小的延时了,采用普通方式延时    
	}
	delay_us((u32)(nms*1000));	      //普通方式延时 
#endif
}

/************************ (C) COPYRIGHT 黑帮老大 *****END OF FILE***************/

			 



































