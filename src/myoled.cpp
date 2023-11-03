#include "myoled.hpp"
#include <stdbool.h>

myOled::myOled(int16_t oledwidth, int16_t oledheight) :myOled_graphics(oledwidth, oledheight)
{
	_OLED_HEIGHT = oledheight;
	_OLED_WIDTH = oledwidth;
	_OLED_PAGE_NUM = (_OLED_HEIGHT/8); 
	bufferWidth = _OLED_WIDTH;
	bufferHeight = _OLED_HEIGHT;
}

void myOled::OLEDbegin( uint16_t I2C_speed , uint8_t I2c_address)
{
	_I2C_speed = I2C_speed;
	_I2C_address = I2c_address;
	OLED_I2C_ON();
	OLEDinit();
	OLED_I2C_OFF();
}

void myOled::OLED_I2C_ON()
{
	if (!bcm2835_i2c_begin())
	{
		printf("Error: Cannot start I2C, Running root?\n");
		return;
	}
	bcm2835_i2c_setSlaveAddress(_I2C_address);  //i2c address
	
	if ( _I2C_speed > 0)  
	{
		// BCM2835_I2C_CLOCK_DIVIDER enum choice  2500 622 150 148
		// Clock divided is based on nominal base clock rate of 250MHz
		bcm2835_i2c_setClockDivider(_I2C_speed);
	} else{
		// default or use set_baudrate instead of clockdivder 100k
		bcm2835_i2c_set_baudrate(100000); //100k baudrate
	}
}

void myOled::OLED_I2C_OFF(void)
{
	bcm2835_i2c_end();
}

void myOled::OLEDinit()
 {

	bcm2835_delay(myOled_INITDELAY);
	myOled_command( myOled_DISPLAY_OFF);
	myOled_command( myOled_SET_DISPLAY_CLOCK_DIV_RATIO);
	myOled_command( 0x80);
	myOled_command( myOled_SET_MULTIPLEX_RATIO );
	myOled_command( _OLED_HEIGHT - 1 );
	myOled_command( myOled_SET_DISPLAY_OFFSET );
	myOled_command(0x00);
	myOled_command( myOled_SET_START_LINE|0x00);
	myOled_command( myOled_CHARGE_PUMP );
	myOled_command(0x14);
	myOled_command( myOled_MEMORY_ADDR_MODE );
	myOled_command(0x00);  //Horizontal Addressing Mode is Used
	myOled_command( myOled_SET_SEGMENT_REMAP| 0x01);
	myOled_command( myOled_COM_SCAN_DIR_DEC );

switch (_OLED_HEIGHT)
{
	case 64: 
		myOled_command( myOled_SET_COM_PINS );
		myOled_command( 0x12 );
		myOled_command( myOled_SET_CONTRAST_CONTROL );
		myOled_command(0xCF);
	break;
	case 32: 
		myOled_command( myOled_SET_COM_PINS );
		myOled_command( 0x02 );
		myOled_command( myOled_SET_CONTRAST_CONTROL );
		myOled_command(0x8F);
	break;
	case 16: // NOTE: not tested, lacking part. 
		myOled_command( myOled_SET_COM_PINS );
		myOled_command( 0x2 ); // ?
		myOled_command( myOled_SET_CONTRAST_CONTROL );
		myOled_command(0xAF);
	break;
}

	myOled_command( myOled_SET_PRECHARGE_PERIOD );
	myOled_command( 0xF1 );
	myOled_command( myOled_SET_VCOM_DESELECT );
	myOled_command( 0x40 );
	myOled_command( myOled_DISPLAY_ALL_ON_RESUME );
	myOled_command( myOled_NORMAL_DISPLAY );
	myOled_command( myOled_DEACTIVATE_SCROLL );
	myOled_command( myOled_DISPLAY_ON );

	bcm2835_delay(myOled_INITDELAY);
}

void myOled::I2C_Write_Byte(uint8_t value, uint8_t cmd)
{
	char buf[2] = {cmd,value};
	uint8_t attemptI2Cwrite = 0;
	uint8_t returnCode = 0;
	
	returnCode = bcm2835_i2c_write(buf, 2); 
	
	while(returnCode != 0)
	{ // failure to write I2C byte 
		attemptI2Cwrite ++;
		printf("Error I2C: Cannot Write byte :: %u\n", attemptI2Cwrite);
		printf("bcm2835I2CReasonCodes :: Error code %u\n", returnCode);
		returnCode  = bcm2835_i2c_write(buf, 2);
		bcm2835_delay(100); // mS
		if (attemptI2Cwrite >= 3) break;
	}
}

void myOled::OLEDupdate()
{
	uint8_t x = 0; uint8_t y = 0; uint8_t w = this->bufferWidth; uint8_t h = this->bufferHeight;
	OLEDBuffer( x,  y,  w,  h, (uint8_t*) this->buffer);
}

void myOled::OLEDclearBuffer()
{

	memset( this->buffer, 0x00, (this->bufferWidth * (this->bufferHeight /8))  );

}

void myOled::OLEDBuffer(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t* data)
{
	OLED_I2C_ON();
	uint8_t tx, ty;
	uint16_t offset = 0;
		
	myOled_command( myOled_SET_COLUMN_ADDR );
	myOled_command(0);   // Column start address (0 = reset)
	myOled_command( _OLED_WIDTH-1 ); // Column end address (127 = reset)

	myOled_command( myOled_SET_PAGE_ADDR );
	myOled_command(0); // Page start address (0 = reset)
	
	switch (_OLED_HEIGHT)
	{
		case 64: myOled_command(7); break;
		case 32: myOled_command(3); break;
		case 16: myOled_command(1); break;
	}
	
	for (ty = 0; ty < h; ty = ty + 8)
		{
		if (y + ty < 0 || y + ty >= _OLED_HEIGHT) {continue;}
		for (tx = 0; tx < w; tx++)
		{

			if (x + tx < 0 || x + tx >= _OLED_WIDTH) {continue;}
			offset = (w * (ty /8)) + tx;
			myOled_data(data[offset++]);
		}
	}
	OLED_I2C_OFF();

}

