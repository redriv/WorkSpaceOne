#include "sys.h"
#include "led.h"
#include "key.h"
#include "oled.h"

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "queue.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
static TaskHandle_t LED_Task_Handle = NULL;/* LED������ */
static TaskHandle_t KEY_Task_Handle = NULL;/* KEY������ */
static TaskHandle_t OLED_Task_Handle = NULL;/* KEY������ */


static void AppTaskCreate(void);/* ���ڴ������� */

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */

static void LED_Task(void* pvParameters);/* LED_Task����ʵ�� */

static void KEY_Task(void* pvParameters);/* KEY_Task����ʵ�� */

static void OLED_Task(void* pvParameters);/* KEY_Task����ʵ�� */



BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
QueueHandle_t Test_Queue =NULL;/* ����һ����Ϣ���о�� */



//�����ʵ�� -�⺯���汾
//STM32F4����ģ��-�⺯���汾
//�Ա����̣�http://mcudev.taobao.com

int main(void)
{ 

	
	BaseType_t xReturn1 = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

	/* ������Ӳ����ʼ�� */
	BSP_Init();

	printf("����һ������[Ұ��]-STM32ȫϵ�п�����-FreeRTOS�̼������̣�\n\n");
	

	/* ����AppTaskCreate���� */
	xReturn1 = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
							(const char*    )"AppTaskCreate",/* �������� */
							(uint16_t       )512,  /* ����ջ��С */
							(void*          )NULL,/* ������ں������� */
							(UBaseType_t    )1, /* ��������ȼ� */
							(TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
	/* ����������� */           
	if(pdPASS == xReturn1)
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
	KEY_Init();
	GPIO_ResetBits(GPIOA,GPIO_Pin_6 );//���øߣ�����
	
	/* ��ʼ��OLED   */
	//OLED_Init();				
	GPIO_ResetBits(GPIOA, GPIO_Pin_7);//���øߣ�����
}


#define QUEUE_LEN 4 /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
#define QUEUE_SIZE 4 /* ������ÿ����Ϣ��С���ֽڣ� */

/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{ 

  taskENTER_CRITICAL();           //�����ٽ���
	
  Test_Queue = xQueueCreate((UBaseType_t)QUEUE_LEN,(UBaseType_t)QUEUE_SIZE);
  if(Test_Queue != NULL)
	printf("����Test_Queue�ɹ�!\r\n");
  
  /* ����LED_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )LED_Task, /* ������ں��� */
                        (const char*    )"LED_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&LED_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����LED_Task����ɹ�!\r\n");
  
  xReturn = xTaskCreate((TaskFunction_t )KEY_Task, 
                        (const char*    )"KEY_Task",
                        (uint16_t       )512,  
                        (void*          )NULL,
                        (UBaseType_t    )3, 
                        (TaskHandle_t*  )&KEY_Task_Handle);
  if(pdPASS == xReturn)
    printf("����KEY_Task����ɹ�!\r\n");
  
  xReturn = xTaskCreate((TaskFunction_t )OLED_Task,  
                        (const char*    )"OLED_Task",
                        (uint16_t       )512,  
                        (void*          )NULL,
                        (UBaseType_t    )4,
                        (TaskHandle_t*  )&OLED_Task_Handle);
  if(pdPASS == xReturn)
    printf("����OLED_Task����ɹ�!\r\n");
  
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

/**********************************************************************
  * @ ������  �� LED_Task
  * @ ����˵���� Test_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void LED_Task(void* parameter)
{	  int count = 0;
	  while (1)
	  {
			if(count >100)count = 0;
			else count++;
		  
			GPIO_ResetBits(GPIOA,GPIO_Pin_6); //LED0��Ӧ����GPIOA.6���ͣ���  ��ͬLED0=0;
			GPIO_SetBits(GPIOA,GPIO_Pin_7);   //LED1��Ӧ����GPIOA.7���ߣ��� ��ͬLED1=1;
			//printf("Test_Task Running,LED1_ON\r\n");
			vTaskDelay(10);   /* ��ʱ500��tick */

			GPIO_SetBits(GPIOA,GPIO_Pin_6);	  //LED0��Ӧ����GPIOA.6���ߣ���  ��ͬLED0=1;
			GPIO_ResetBits(GPIOA,GPIO_Pin_7); //LED1��Ӧ����GPIOA.7���ͣ��� ��ͬLED1=0; 
			//printf("Test_Task Running,LED1_OFF\r\n");
			vTaskDelay(50);   /* ��ʱ500��tick */

			xReturn = xQueueSend( Test_Queue, /*  ��Ϣ���еľ�� */
									   &count,/*  ���͵���Ϣ���� */
										 0 ); /*  �ȴ�ʱ�� 0 */
			if (pdPASS != xReturn)	printf(" ��Ϣ send_data1  ����ʧ��!\n\n");
			
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
			case KEY0_PRES:	//����LED0��ת
			{
					printf("����Test����\n");
					vTaskSuspend(LED_Task_Handle);/* ����LED���� */
					printf("����Test����ɹ���\n");
					break;
			}
			case KEY1_PRES:	//����LED1��ת	
			{					
					printf("�ָ�Test����\n");
					vTaskResume(LED_Task_Handle);/* �ָ�LED���� */
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

/**********************************************************************
  * @ ������  �� OLED_Task
  * @ ����˵���� OLED_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void OLED_Task(void* parameter)
{
	int r_queue; /* ����һ��������Ϣ�ı��� */
 
	while(1)
	{
		
		xReturn = xQueueReceive( Test_Queue, /*  ��Ϣ���еľ�� */
								   &r_queue, /*  ���͵���Ϣ���� */
						     portMAX_DELAY); /*  �ȴ�ʱ��  һֱ�� */
		if (pdTRUE== xReturn)
			printf(" ���ν��յ��������ǣ�%d\n\n",r_queue);
		else
			printf(" ���ݽ��ճ���, �������: 0x%lx\n",xReturn);

		//OLED_ShowString(0,10,"hello FreeRtos!!!",12);
		//vTaskDelay(20);/* ��ʱ20��tick */
	}

}


