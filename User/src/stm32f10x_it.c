/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
//#include "hw_config.h"
//#include "usb_lib.h"
//#include "usb_istr.h"

#include "sys.h"

 
void NMI_Handler(void)
{
	#if MY_DEBUG==1
	Usart_SetData(0,(u8*)"NMI_H\r\n",strlen("NMI_H\r\n"));
	#endif
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
	#if MY_DEBUG==1
	Usart_SetData(0,(u8*)"HardFault_H\r\n",strlen("HardFault_H\r\n"));
	#endif
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
	#if MY_DEBUG==1
	Usart_SetData(0,(u8*)"MemManage_H\r\n",strlen("MemManage_H\r\n"));
	#endif
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
	#if MY_DEBUG==1
	Usart_SetData(0,(u8*)"BusFault_H\r\n",strlen("BusFault_H\r\n"));
	#endif
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
	#if MY_DEBUG==1
	Usart_SetData(0,(u8*)"UsageFault_H\r\n",strlen("UsageFault_H\r\n"));
	#endif
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
	#if MY_DEBUG==1
	Usart_SetData(0,(u8*)"SVC_H\r\n",strlen("SVC_H\r\n"));
	#endif
}
 
void DebugMon_Handler(void)
{
	#if MY_DEBUG==1
	Usart_SetData(0,(u8*)"DebugMon_H\r\n",strlen("DebugMon_H\r\n"));
	#endif
}

/*******************************************************************************
* Function Name  : USB_IRQHandler
* Description    : This function handles USB Low Priority interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD)|| defined(STM32L1XX_MD_PLUS)|| defined (STM32F37X)
void USB_LP_IRQHandler(void)
#else
void USB_LP_CAN1_RX0_IRQHandler(void)
#endif
{
	//OSIntEnter();		    //Ω¯»Î÷–∂œ
	#if MY_DEBUG==1
	Usart_SetData(0,(u8*)"USB_LP\r\n",strlen("USB_LP\r\n"));
	#endif
	//USB_Istr();
	//OSIntExit(); 
}

/*******************************************************************************
* Function Name  : USB_FS_WKUP_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*
#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD)|| defined(STM32L1XX_MD_PLUS)
void USB_FS_WKUP_IRQHandler(void)
#else
void USBWakeUp_IRQHandler(void)
#endif
{
	//OSIntEnter();
	#if MY_DEBUG==1
	Usart_SetData(0,(u8*)"USBW\r\n",strlen("USBW\r\n"));
	#endif
	//EXTI_ClearITPendingBit(EXTI_Line18);
	//OSIntExit(); 
}*/
/* 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}
*/
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
