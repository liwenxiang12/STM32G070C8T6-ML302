#include "AHT20.h"
#include "bsp_i2c.h"
#include "stdio.h"

uint8_t AHT20_Read_Status(void)//��ȡAHT10��״̬�Ĵ���
{
    uint8_t Byte_first;
    Sensors_I2C_ReadRegister(AHT20_SLAVE_ADDRESS, 0x00, 1,&Byte_first);

	return Byte_first;
}

uint8_t AHT20_Read_Cal_Enable(void)
{
    uint8_t val = 0;//ret = 0,

    val = AHT20_Read_Status();
    if((val & 0x68) == 0x08)  //�ж�NORģʽ��У׼����Ƿ���Ч
        return 1;
    else
        return 0;
}

void AHT20_Read_CTdata(uint32_t *ct) //��ȡAHT10���¶Ⱥ�ʪ������
{
    uint32_t RetuData = 0;
	uint16_t cnt = 0;
    uint8_t Data[10];
    uint8_t tmp[10];

    tmp[0] = 0x33;
    tmp[1] = 0x00;
	Sensors_I2C_WriteRegister(AHT20_SLAVE_ADDRESS,StartTest, 2, tmp);  //P0 ���жϲ�ʹ��
	HAL_Delay(80);//�ȴ�80ms

    cnt = 0;
	while(((AHT20_Read_Status()&0x80) == 0x80))//�ȴ�æ״̬����
	{
        HAL_Delay(1);
        if(cnt++ >= 100)
        {
            break;
        }
	}

    Sensors_I2C_ReadRegister(AHT20_SLAVE_ADDRESS, 0x00, 7,Data);

	RetuData = 0;
  RetuData = (RetuData|Data[1]) << 8;
	RetuData = (RetuData|Data[2]) << 8;
	RetuData = (RetuData|Data[3]);
	RetuData = RetuData >> 4;
	ct[0] = RetuData;

    RetuData = 0;
	RetuData = (RetuData|Data[3]) << 8;
	RetuData = (RetuData|Data[4]) << 8;
	RetuData = (RetuData|Data[5]);
	RetuData = RetuData&0xfffff;
	ct[1] = RetuData;
}

uint8_t count;
uint8_t AHT20_Init(void)
{
    uint8_t tmp[10];

    HAL_Delay(40);

    tmp[0] = 0x08;
    tmp[1] = 0x00;
	  Sensors_I2C_WriteRegister(AHT20_SLAVE_ADDRESS,INIT, 2, tmp);  //P0 ���жϲ�ʹ��

    HAL_Delay(500);
    count = 0;

    while(AHT20_Read_Cal_Enable() == 0)//��Ҫ�ȴ�״̬��status��Bit[3]=1ʱ��ȥ�����ݡ����Bit[3]������1 ����������λ0xBA��AHT10�������³�ʼ��AHT10��ֱ��Bit[3]=1
    {
        Sensors_I2C_WriteRegister(AHT20_SLAVE_ADDRESS,SoftReset, 0, tmp);
        HAL_Delay(200);

        Sensors_I2C_WriteRegister(AHT20_SLAVE_ADDRESS,INIT, 2, tmp);

        count++;
        if(count >= 10)
            return 0;
        HAL_Delay(500);
    }
    return 1;
}