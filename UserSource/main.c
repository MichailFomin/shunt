#include "stm32f0xx.h" 
#include "Init_I2C.h"
#include "SSD1306_I2C.h"
#include "stdlib.h"
#include "fonts.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_flash.h"


uint8_t num = 0,i_temp1,i_temp2;
uint32_t ADC_Value1 = 0, ADC_Value2 = 0;
float mV1 = 0, mV1_slow = 0, mV2 = 0, mV2_slow = 0;
float Temp1 = 0, Temp2 = 0,max = 0;
//uint32_t ADC_Value_sum1 = 0, ADC_Value_sum2 = 0;
//uint16_t ADC_i1 = 0,ADC_i2 = 0;
uint16_t ACP[2],i = 0;
//float ADC_sum[3],ADC_RMS[3],ADC_RMS_avr[3],ADC_RMS_avr_tmp[3],divider_rms = 0;
//uint32_t sec = 0,num_sample = 0,max_num_sample = 0,msec = 0,msec20 = 0,rms_sample = 0,max_rms_sample = 0;
float    mV1_mas[30],mV2_mas[30],mV1_sr = 0,mV2_sr = 0;

char str[15],str2[15],str3[15],str4[15];
RCC_ClocksTypeDef RCC_Clocks;

void Init_Port(void)
{
   RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
   
//   00: Input mode (reset state)
//   01: General purpose output mode
//   10: Alternate function mode
//   11: Analog mode
   GPIOA->MODER |= (    GPIO_MODER_MODER5
                     |  GPIO_MODER_MODER6
//                     | GPIO_MODER_MODER4
                     );
   
   //11: High speed
   GPIOA->OSPEEDR |= (  GPIO_OSPEEDR_OSPEEDR5
                     |  GPIO_OSPEEDR_OSPEEDR6
                     );
   
   //00: No pull-up, pull-down
   //01: Pull-up
   //10: Pull-down
   GPIOA->PUPDR &= ~(   GPIO_PUPDR_PUPDR5_1
                     |  GPIO_PUPDR_PUPDR6_1
                     );
}

void ADC_init(void)
{
   RCC->APB2ENR |= RCC_APB2ENR_ADCEN; // включаем тактирование АЦП
   
   ADC1->CR |= ADC_CR_ADCAL; // Запуск калибровки 
   while ((ADC1->CR & ADC_CR_ADCAL)) ; //ждем окончания калибровки
   
   //ADC1->ISR |= ADC_ISR_EOC; // ФЛАГ по окончании преобразования
   ADC1->IER |= ADC_IER_EOCIE; // прерывание по окончании преобразования
   ADC1->SMPR |= ADC_SMPR_SMP_0; // 7.5 длительность выборки
   ADC1->CHSELR |= ADC_CHSELR_CHSEL5 | ADC_CHSELR_CHSEL6;// | ADC_CHSELR_CHSEL3 | ADC_CHSELR_CHSEL4;
   ADC1->CFGR1 |= //ADC_CFGR1_DISCEN
                 ADC_CFGR1_DMACFG
               | ADC_CFGR1_DMAEN
//               | ADC_CFGR1_CONT
               ;
//   NVIC_EnableIRQ(ADC1_IRQn); // включаем прерывания АЦП
   
   ADC1->CR |= ADC_CR_ADEN; // включаем АЦП
   while ( !(ADC1->ISR & ADC_ISR_ADRDY) ) ; // ждем готовности АЦП
   ADC1->CR |= ADC_CR_ADSTART; // запускаем преобразование
   
}

void DMA1_Channel1_IRQHandler(void)
{
   if ( (DMA1->ISR & DMA_ISR_TCIF1) )
   {
      DMA1->IFCR |= DMA_IFCR_CTCIF1;	// clear flag TC
//      switch(num) {
//         case 0:
            mV1 = ACP[0]; // читаем значение преобразования
//            break;
//         case 1:
            mV2 = ACP[1]; // читаем значение преобразования
//						ADC_sum[2]+= ACP[2]; // читаем значение преобразования
//						ADC_sum[3]+= ACP[3]; // читаем значение преобразования
//            break;
//      }
//      num++;
//      if (num > 1) num = 0;
//      ADC_sum[0]+= ACP[0];
//      ADC_sum[1]+= ACP[1];
//      ADC_sum[2]+= ADC[2];
      
      
//      rms_sample++;
//      num_sample++;
      

      
      ADC1->CR |= ADC_CR_ADSTART; // запускаем преобразование
      
   }
}

void DMA_ini_ADC1(void)
{
   

   
   RCC->AHBENR    |= RCC_AHBENR_DMA1EN;
   
   DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;      //peripheral address
   DMA1_Channel1->CMAR = (uint32_t)&ACP[0];
   DMA1_Channel1->CNDTR = 2;
   
   DMA1_Channel1->CCR |= DMA_CCR_PL_0     //Channel priority level - medium
                     | DMA_CCR_MSIZE_0    //Memory size 16
                     | DMA_CCR_PSIZE_0    //Peripheral size 16
                     | DMA_CCR_MINC       //Memory increment mode
                     | DMA_CCR_CIRC     //Circular mode
                     | DMA_CCR_TCIE    //Transfer complete interrupt enable
                     | DMA_CCR_EN      //Channel enable
                     ;
   NVIC_EnableIRQ(DMA1_Channel1_IRQn);
   NVIC_SetPriority(DMA1_Channel1_IRQn,1);
}

//void ADC1_IRQHandler(void)
//{
//   if ( (ADC1->ISR & ADC_ISR_EOC) )
//   {
//      switch(num) {
//         case 0:
//            ACP[0] = ADC1->DR; // читаем значение преобразования
//            break;
//         case 1:
//            ACP[1] = ADC1->DR; // читаем значение преобразования
//            break;
//      }
//      num++;
//      if (num > 1) num = 0;
//      
//      ADC1->ISR |= ADC_ISR_EOC;// сбрасываем флаг 
//      
//   }
//}

//void ADC_init(void)
//{
//   RCC->APB2ENR |= RCC_APB2ENR_ADCEN; // включаем тактирование АЦП
//   
//   ADC1->CR |= ADC_CR_ADCAL; // Запуск калибровки 
//   while ((ADC1->CR & ADC_CR_ADCAL)) ; //ждем окончания калибровки
//   
//   //ADC1->ISR |= ADC_ISR_EOC; // ФЛАГ по окончании преобразования
//   ADC1->IER |= ADC_IER_EOCIE; // прерывание по окончании преобразования
////   ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2; // 7.5 длительность выборки
//   ADC1->CHSELR |= ADC_CHSELR_CHSEL5 | ADC_CHSELR_CHSEL6;
//   ADC1->CFGR1 |= ADC_CFGR1_DISCEN
//               | ADC_CFGR1_DMACFG
//               | ADC_CFGR1_DMAEN
//               ;
//   NVIC_EnableIRQ(ADC1_IRQn); // включаем прерывания АЦП
//   
//   ADC1->CR |= ADC_CR_ADEN; // включаем АЦП
//   while ( !(ADC1->ISR & ADC_ISR_ADRDY) ) ; // ждем готовности АЦП
//   ADC1->CR |= ADC_CR_ADSTART; // запускаем преобразование
//   
//}

//void DMA1_Channel1_IRQHandler(void)
//{
//   if ( (DMA1->ISR & DMA_ISR_TCIF1) )
//   {
//      DMA1->IFCR |= DMA_IFCR_CTCIF1;	// clear flag TC
//      
//      ADC_sum[0]+= ACP[0];
//      ADC_sum[1]+= ACP[1];
////      ADC_sum[2]+= ADC[2];
//      
//      
//      rms_sample++;
//      num_sample++;
//      ADC1->CR |= ADC_CR_ADSTART; // запускаем преобразование
//      
//   }
//}

//void DMA_ini_ADC1(void)
//{
//   RCC->AHBENR    |= RCC_AHBENR_DMA1EN;
//   
//   DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;      //peripheral address
//   DMA1_Channel1->CMAR = (uint32_t)&ACP[0];
//   DMA1_Channel1->CNDTR = 2;
//   
//   DMA1_Channel1->CCR |= DMA_CCR_PL_0     //Channel priority level - medium
//                     | DMA_CCR_MSIZE_0    //Memory size 16
//                     | DMA_CCR_PSIZE_0    //Peripheral size 16
//                     | DMA_CCR_MINC       //Memory increment mode
//                     | DMA_CCR_CIRC     //Circular mode
//                     | DMA_CCR_TCIE    //Transfer complete interrupt enable
//                     | DMA_CCR_EN      //Channel enable
//                     ;
//   NVIC_EnableIRQ(DMA1_Channel1_IRQn);
//   NVIC_SetPriority(DMA1_Channel1_IRQn,1);
//}

//void SysTick_Handler (void) 
//{
////   if ( flag ) 
////      {
////         flag = 0;
////         ADC1->CR2 |= ADC_CR2_SWSTART; // Start conversion of injected channels
////      } 
//   msec++;
//   msec20++;
//   if (msec20 == 20) 
//   {
//      msec20 = 0;
//      
////      ADC_RMS[0] = ADC_sum[0] / rms_sample;
////      ADC_sum[0] = 0;
////      ADC_RMS[1] = ADC_sum[1] / rms_sample;
////      ADC_sum[1] = 0;
//////      ADC_RMS[2] = ADC_sum[2] / rms_sample;
//////      ADC_sum[2] = 0;
////      max_rms_sample = rms_sample;
////      rms_sample = 0;
////      
////      ADC_RMS_avr_tmp[0] += ADC_RMS[0] ;
////      ADC_RMS_avr_tmp[1] += ADC_RMS[1] ;
//////      ADC_RMS_avr_tmp[2] += ADC_RMS[2] ;
////      i++;
////      if (i == 20) 
////      {
////         ADC_RMS_avr[0] = ADC_RMS_avr_tmp[0]*0.05;
////         ADC_RMS_avr_tmp[0] = 0;
////         ADC_RMS_avr[1] = ADC_RMS_avr_tmp[1]*0.05;
////         ADC_RMS_avr_tmp[1] = 0;
//////         ADC_RMS_avr[2] = ADC_RMS_avr_tmp[2]*0.05;
//////         ADC_RMS_avr_tmp[2] = 0;
////         i = 0;
////      }
//      
//      
//   }
//   
//   if (msec == 1000) 
//   {
//      sec++;
//      msec = 0;
//      max_num_sample = num_sample;
//      
//      num_sample = 0;
//      if (sec == 60) 
//      {
//         sec = 0;
//         
//      }
//   }
//   
//   
//   
//}

void SetSysClockToHSI48(void)
{
   uint32_t StartUpCounter = 0, HSIStatus = 0;
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/    
  /* Enable HSE */    
//  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSIStatus = RCC->CR & RCC_CR_HSIRDY;
    StartUpCounter++;  
  } while((HSIStatus == 0) && (StartUpCounter != HSI_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSIRDY) != RESET)
  {
    HSIStatus = (uint32_t)0x01;
  }
  else
  {
    HSIStatus = (uint32_t)0x00;
  }  

  if (HSIStatus == (uint32_t)0x01)
  {

    /* Enable Prefetch Buffer */
    //FLASH->ACR |= FLASH_ACR_PRFTBE;
		FLASH_PrefetchBufferCmd(ENABLE);
		
    /* Flash 0 wait state */
//    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
//    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;    
//FLASH->ACR |= (uint32_t)FLASH_Latency_2;
//	FLASH_SetLatency(FLASH_Latency_2);
//    /* HCLK = SYSCLK */
//    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
//      
//    /* PCLK2 = HCLK */
//    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
//    
//    /* PCLK1 = HCLK */
//    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;
//    
		/*  PLL configuration:  = (HSI / 2) * 12 = 48 MHz */
		RCC_PREDIV1Config(RCC_PREDIV1_Div1);
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
		

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
  } 
}



int main(void)
{
   RCC_DeInit();

   SetSysClockToHSI48();
   
//   RCC_GetClocksFreq (&RCC_Clocks);
//	Delay_ms(1000);
//   SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000); // 1 ms

   Init_I2C();
   Init_SSD1306(SSD1306_COLOR_BLACK);
//   DMA_ini_ADC1();
//   ADC_init();
   Init_Port();
   ADC_init();
   DMA_ini_ADC1();
   
   while(1) 
   {
//      ADC1->CR |= ADC_CR_ADSTART; // запускаем преобразование 
      
      mV1 = (ACP[0]) * (3.34/4096);  
      mV2 = ACP[1] * (3.34/4096);  
//      ADC_Value_sum1 += ACP[0];
//      ADC_Value_sum2 += ACP[1];
//      ADC_i1++;
//      ADC_i2++;
//            
//      if (ADC_i1 == 10) {
//         mV1_slow = (ADC_Value_sum1 *0.1) * (3.3/4096);  
//         ADC_Value_sum1 = 0;
//         ADC_i1 = 0;
//      }  
//      if (ADC_i2 == 10) {
//         mV2_slow = (ADC_Value_sum2 *0.1) * (3.3/4096);  
//         ADC_Value_sum2 = 0;
//         ADC_i2 = 0;
//      } 
      
      mV1_mas[i_temp1] = mV1;
      mV2_mas[i_temp1] = mV2;
      i_temp1++;
      if (i_temp1 == 15) i_temp1 = 0;
      for(i = 0;i < 15; i++)
      {
         mV1_sr += mV1_mas[i];
         mV2_sr += mV2_mas[i];
      }
      mV1_sr /= 15 ;
      mV2_sr /= 15;
      
//      Temp1 = (mV1 - 0.5) * 100;
//      Temp2 = (mV2 - 0.5) * 100;
      Temp1 = mV2_sr * 10 ;
      Temp2 = mV1_sr / 19.6 ;
//      if (mV1_sr < 0.0015) Temp2 = 0; else 
      if (max < Temp2) max = Temp2;
       
      sprintf(str, "V1 = %2.2f ",Temp1);
      sprintf(str2, "V2 = %2.4f ",Temp2);
      sprintf(str3, "max = %2.2f    ",max);
      sprintf(str4, "%3.2f %3.2f   ",Temp2/0.0025,Temp2/0.00015); // *400 for 30A - 0,0025 // *4000 - 500A 0,00015
 /*
      для шунта 500 А 
      0.075 / 500 = 0.00015
      для шунта 30 А
      0.075 / 30 = 0.025
      */     
      SSD1306_GotoXY(0, 0);
      SSD1306_Puts(str, &Font_7x10, SSD1306_COLOR_WHITE);
      SSD1306_GotoXY(0, 12);
      SSD1306_Puts(str2, &Font_7x10, SSD1306_COLOR_WHITE);
      SSD1306_GotoXY(0, 22);
      SSD1306_Puts(str3, &Font_11x18, SSD1306_COLOR_WHITE);
      SSD1306_GotoXY(5, 42);
      SSD1306_Puts(str4, &Font_11x18, SSD1306_COLOR_WHITE);


//      sprintf(str,  "V1 = %2.3f  ",ADC_RMS_avr[0]);
//      sprintf(str2, "V2 = %2.3f  ",ADC_RMS_avr[1]);
//      sprintf(str3, "max = %2.3f    ",max_num_sample); 
      
//      SSD1306_GotoXY(0, 0);
//      SSD1306_Puts(str, &Font_7x10, SSD1306_COLOR_WHITE);
//      SSD1306_GotoXY(0, 12);
//      SSD1306_Puts(str2, &Font_7x10, SSD1306_COLOR_WHITE);
//      SSD1306_GotoXY(0, 22);
//      SSD1306_Puts(str3, &Font_11x18, SSD1306_COLOR_WHITE);
      
      SSD1306_UpdateScreen_My();
   }

}

