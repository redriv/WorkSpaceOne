#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"

#include "FreeRTOS.h"
#include "task.h"


static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
static TaskHandle_t Test_Task_Handle = NULL;/* LED������ */
static TaskHandle_t KEY_Task_Handle = NULL;/* KEY������ */


static void AppTaskCreate(void);/* ���ڴ������� */
static void Test_Task(void* pvParameters);/* Test_Task����ʵ�� */
static void KEY_Task(void* pvParameters);/* KEY_Task����ʵ�� */
static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */


//�����ʵ�� -�⺯���汾
//STM32F4����ģ��-�⺯���汾
//�Ա����̣�http://mcudev.taobao.com

int main(void)
{ 

	
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

	/* ������Ӳ����ʼ�� */
	BSP_Init();

	printf("����һ������[Ұ��]-STM32ȫϵ�п�����-FreeRTOS�̼������̣�\n\n");
	

	/* ����AppTaskCreate���� */
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
							(const char*    )"AppTaskCreate",/* �������� */
							(uint16_t       )512,  /* ����ջ��С */
							(void*          )NULL,/* ������ں������� */
							(UBaseType_t    )1, /* ��������ȼ� */
							(TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
	/* ����������� */           
	if(pdPASS == xReturn)
	vTaskStartScheduler();   /* �������񣬿������� */
	else
	return -1;  

	while(1);   /* ��������ִ�е����� */    
}



/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED ��ʼ�� */
	LED_GPIO_Config();

	/* ���ڳ�ʼ��	*/
	Debug_USART_Config();
	
  
	/* ������ʼ��	*/
	//Key_GPIO_Config();
	KEY_Init();
	
	delay_init(168);		  //��ʼ����ʱ����
	
	//OLED_Init();				//��ʼ��OLED

}
/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
  
  /* ����Test_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Test_Task, /* ������ں��� */
                        (const char*    )"Test_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&Test_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����Test_Task����ɹ�!\r\n");
  /* ����KEY_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )KEY_Task,  /* ������ں��� */
                        (const char*    )"KEY_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )3, /* ��������ȼ� */
                        (TaskHandle_t*  )&KEY_Task_Handle);/* ������ƿ�ָ�� */ 
  if(pdPASS == xReturn)
    printf("����KEY_Task����ɹ�!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

/**********************************************************************
  * @ ������  �� Test_Task
  * @ ����˵���� Test_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Test_Task(void* parameter)
{	
	  while (1)
	  {
		GPIO_ResetBits(GPIOA,GPIO_Pin_6); //LED0��Ӧ����GPIOA.6���ͣ���  ��ͬLED0=0;
		GPIO_SetBits(GPIOA,GPIO_Pin_7);   //LED1��Ӧ����GPIOA.7���ߣ��� ��ͬLED1=1;
		printf("Test_Task Running,LED1_ON\r\n");
		vTaskDelay(500);   /* ��ʱ500��tick */
		
		GPIO_SetBits(GPIOA,GPIO_Pin_6);	  //LED0��Ӧ����GPIOA.6���ߣ���  ��ͬLED0=1;
		GPIO_ResetBits(GPIOA,GPIO_Pin_7); //LED1��Ӧ����GPIOA.7���ͣ��� ��ͬLED1=0; 
		printf("Test_Task Running,LED1_OFF\r\n");
		vTaskDelay(500);   /* ��ʱ500��tick */
	  }
}

/**********************************************************************
  * @ ������  �� Test_Task
  * @ ����˵���� Test_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void KEY_Task(void* parameter)
{	u8 key;           //�����ֵ
	while(1)
	{
		key=KEY_Scan(0);		//�õ���ֵ
		switch(key)
					{				 
			//case WKUP_PRES:	//���Ʒ�����
			//{
			//	LED0=!LED0;
			//	LED1=!LED1;
			//	printf("ide Test����\n");
			//	break;
			//}
			case KEY0_PRES:	//����LED0��ת
			{
					printf("����Test����\n");
					vTaskSuspend(Test_Task_Handle);/* ����LED���� */
					printf("����Test����ɹ���\n");
					break;
			}
			case KEY1_PRES:	//����LED1��ת	
			{					
					printf("�ָ�Test����\n");
					vTaskResume(Test_Task_Handle);/* �ָ�LED���� */
					printf("�ָ�Test����ɹ���\n");
					break;
			}
			default:
				//printf("ide Test����\n");
				vTaskDelay(200);/* ��ʱ20��tick */
				break;
					
		}
	}
}


