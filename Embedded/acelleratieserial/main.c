#include "driverlib.h"
#include "i2c.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


#define slave_adress_MPU6050       0x68
#define MPU6050_MODULE     EUSCI_B0_MODULE
#define UART_MODULE		   EUSCI_A0_MODULE

uint8_t PWR_MGMT_1=0x6B;
uint8_t MPU6050_A_X_LOW=0x3C;
uint8_t MPU6050_A_X_HIGH=0x3B;
uint8_t MPU6050_A_Y_LOW=0x3E;
uint8_t MPU6050_A_Y_HIGH=0x3D;
uint8_t MPU6050_A_Z_LOW=0x40;
uint8_t MPU6050_A_Z_HIGH=0x3F;
uint8_t ACCEL_CONFIG=0x1C;


const eUSCI_I2C_MasterConfig i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        78,                                     // BRDIV = 78
        2,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // MSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};



 void INIT_MPU6050_MODULE()
 {

	    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
	        	 GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

	    /* Initializing I2C Master to SMCLK at 100kbs with no autostop */
	   MAP_I2C_initMaster(MPU6050_MODULE, &i2cConfig);
	    /* Specify slave address */
	   MAP_I2C_setSlaveAddress(MPU6050_MODULE,slave_adress_MPU6050);
	    /* Set Master in transmit mode */
	   MAP_I2C_setMode(MPU6050_MODULE, EUSCI_B_I2C_TRANSMIT_MODE);
	    /* Enable I2C Module to start operations */
	   MAP_I2C_enableModule(MPU6050_MODULE);
	   MAP_I2C_masterSendMultiByteStart(MPU6050_MODULE, PWR_MGMT_1);
	   _delay_cycles(100);
	   MAP_I2C_masterSendMultiByteNext(MPU6050_MODULE, 0);
	   MAP_I2C_masterSendMultiByteStop(MPU6050_MODULE);

 }

void INIT_UART_MODULE()
{
		 /* Selecting P1.2 and P1.3 in UART mode */
		    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
		    		GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
		    /* Setting DCO to 12MHz */
		    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
		    /* Configuring UART Module */
		    UART_initModule(UART_MODULE, &uartConfig);
		    /* Enable UART module */
		    UART_enableModule(UART_MODULE);

}

int32_t MPU6050_GET_X()
 {
 	static volatile uint8_t RXData_MPU6050_A_X_LOW=0;
 	static volatile uint8_t RXData_MPU6050_A_X_HIGH=0;

 	static int32_t RXData_MPU6050_A_X_HIGHLOW=0;
  //	static int32_t POZ=32768;
  //	static int32_t MAX=65536;
 //static int32_t LBS=16384;

 	while (MAP_I2C_masterIsStopSent(EUSCI_B0_MODULE) == EUSCI_B_I2C_SENDING_STOP);
 	I2C_masterSendSingleByte(MPU6050_MODULE, MPU6050_A_X_LOW);
 	while (MAP_I2C_masterIsStopSent(EUSCI_B0_MODULE) == EUSCI_B_I2C_SENDING_STOP);
 	RXData_MPU6050_A_X_LOW=I2C_masterReceiveSingleByte(MPU6050_MODULE);
 	while (MAP_I2C_masterIsStopSent(EUSCI_B0_MODULE) == EUSCI_B_I2C_SENDING_STOP);
 	I2C_masterSendSingleByte(MPU6050_MODULE, MPU6050_A_X_HIGH);
 	while (MAP_I2C_masterIsStopSent(EUSCI_B0_MODULE) == EUSCI_B_I2C_SENDING_STOP);
 	RXData_MPU6050_A_X_HIGH=I2C_masterReceiveSingleByte(MPU6050_MODULE);

 	RXData_MPU6050_A_X_HIGHLOW=(RXData_MPU6050_A_X_HIGH<<8)|(RXData_MPU6050_A_X_LOW);
 	//RXData_MPU6050_A_X_HIGHLOW=(RXData_MPU6050_A_X_HIGHLOW*100/LBS);
 	return RXData_MPU6050_A_X_HIGHLOW;


	/*if(RXData_MPU6050_A_X_HIGHLOW>POZ)
	{
		RXData_MPU6050_A_X_HIGHLOW=((RXData_MPU6050_A_X_HIGHLOW-MAX)*1000/LBS);
		return RXData_MPU6050_A_X_HIGHLOW;
	}
	else
	{
		RXData_MPU6050_A_X_HIGHLOW=(RXData_MPU6050_A_X_HIGHLOW*1000/LBS);
	return RXData_MPU6050_A_X_HIGHLOW;
	}*/
 }


int32_t MPU6050_GET_Y()
{
	static volatile uint8_t RXData_MPU6050_A_Y_LOW=0;
	static volatile uint8_t RXData_MPU6050_A_Y_HIGH=0;
	static int32_t RXData_MPU6050_A_Y_HIGHLOW=0;

	while (MAP_I2C_masterIsStopSent(EUSCI_B0_MODULE) == EUSCI_B_I2C_SENDING_STOP);
	I2C_masterSendSingleByte(MPU6050_MODULE, MPU6050_A_Y_LOW);
	while (MAP_I2C_masterIsStopSent(EUSCI_B0_MODULE) == EUSCI_B_I2C_SENDING_STOP);
	RXData_MPU6050_A_Y_LOW=I2C_masterReceiveSingleByte(MPU6050_MODULE);

	while (MAP_I2C_masterIsStopSent(EUSCI_B0_MODULE) == EUSCI_B_I2C_SENDING_STOP);
	I2C_masterSendSingleByte(MPU6050_MODULE, MPU6050_A_Y_HIGH);
	while (MAP_I2C_masterIsStopSent(EUSCI_B0_MODULE) == EUSCI_B_I2C_SENDING_STOP);
	RXData_MPU6050_A_Y_HIGH=I2C_masterReceiveSingleByte(MPU6050_MODULE);

	RXData_MPU6050_A_Y_HIGHLOW=(RXData_MPU6050_A_Y_HIGH<<8)|(RXData_MPU6050_A_Y_LOW);
	return RXData_MPU6050_A_Y_HIGHLOW;
}

int32_t MPU6050_GET_Z()
{
	static volatile uint8_t RXData_MPU6050_A_Z_LOW=0;
	static volatile uint8_t RXData_MPU6050_A_Z_HIGH=0;
	static uint32_t RXData_MPU6050_A_Z_HIGHLOW=0;

	while (MAP_I2C_masterIsStopSent(EUSCI_B0_MODULE) == EUSCI_B_I2C_SENDING_STOP);
	I2C_masterSendSingleByte(MPU6050_MODULE, MPU6050_A_Z_LOW);
	while (MAP_I2C_masterIsStopSent(EUSCI_B0_MODULE) == EUSCI_B_I2C_SENDING_STOP);
	RXData_MPU6050_A_Z_LOW=I2C_masterReceiveSingleByte(MPU6050_MODULE);

	while (MAP_I2C_masterIsStopSent(EUSCI_B0_MODULE) == EUSCI_B_I2C_SENDING_STOP);
	I2C_masterSendSingleByte(MPU6050_MODULE, MPU6050_A_Z_HIGH);
	while (MAP_I2C_masterIsStopSent(EUSCI_B0_MODULE) == EUSCI_B_I2C_SENDING_STOP);
	RXData_MPU6050_A_Z_HIGH=I2C_masterReceiveSingleByte(MPU6050_MODULE);

	RXData_MPU6050_A_Z_HIGHLOW=(RXData_MPU6050_A_Z_HIGH<<8)|(RXData_MPU6050_A_Z_LOW);
	return RXData_MPU6050_A_Z_HIGHLOW;
}

int32_t CONVERSION(VALUE)
{
	const int32_t POZ=32768;
	const int32_t MAX=65536;
	const int32_t LBS=16384;

	if(VALUE>POZ)
		{
			VALUE=((VALUE-MAX)*1000/LBS);
			return VALUE;
		}

	else
		{
			VALUE=(VALUE*1000/LBS);
			return VALUE;
		}
}

void printCaracter(char c){
	UART_transmitData(EUSCI_A0_MODULE,c);
}

void printString(char* myString){
     while(*myString!=0){
    	UART_transmitData(EUSCI_A0_MODULE,*myString);
    	myString++;
     }
   }



void print(int number)
{
	char neg=('-');
	char value[10];
	int i=0;

	if(number>0)
	{
		do
			  {
					value[i++] = (char)(number % 10) + '0'; //convert integer to character
					number /= 10;
			  } while(number);

		while(i)
		{
			printCaracter(value[--i]);
		}
	}
	else
	{
		do
		{
			number=abs(number);
			value[i++] = (char)(number % 10) + '0'; //convert integer to character
			number /= 10;
		} while(number);
		printCaracter(neg);
		while(i)
		{
		printCaracter(value[--i]);
		}
	}
}


void printxyz(int a, int b, int c)
{
	print(a);
	printString(";");
	print(b);
	printString(";");
	print(c);
	//printString(";");
	printCaracter(10);
	printCaracter(13);
}

void PRINT()
{
	uint32_t i;
	static int32_t MPU6050_X_VALUE=0;
	static int CONVERSION_X_VALUE=0;

	static int32_t MPU6050_Y_VALUE=0;
	static int CONVERSION_Y_VALUE=0;

	static int32_t MPU6050_Z_VALUE=0;
	static int CONVERSION_Z_VALUE=0;

	MPU6050_X_VALUE=MPU6050_GET_X();
	CONVERSION_X_VALUE=CONVERSION(MPU6050_X_VALUE);

	MPU6050_Y_VALUE=MPU6050_GET_Y();
	CONVERSION_Y_VALUE=CONVERSION(MPU6050_Y_VALUE);

	MPU6050_Z_VALUE=MPU6050_GET_Z();
	CONVERSION_Z_VALUE=CONVERSION(MPU6050_Z_VALUE);

	printxyz(CONVERSION_X_VALUE, CONVERSION_Y_VALUE, CONVERSION_Z_VALUE);

	for(i=0;i<=100000;i++);
}





int main(void)
{
	MAP_WDT_A_holdTimer();
    INIT_MPU6050_MODULE();
    INIT_UART_MODULE();

    while(1)
    {
    	PRINT();
    }
}




