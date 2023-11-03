#pragma once

// Library includes

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <bcm2835.h>
#include "myOled_OLED_graphics.hpp"


//  myOled Command Set  

// Fundamental Commands
#define myOled_SET_CONTRAST_CONTROL    0x81
#define myOled_DISPLAY_ALL_ON_RESUME    0xA4
#define myOled_DISPLAY_ALL_ON                   0xA5
#define myOled_NORMAL_DISPLAY                  0xA6
#define myOled_INVERT_DISPLAY                    0xA7
#define myOled_DISPLAY_OFF                         0xAE
#define myOled_DISPLAY_ON                           0xAF
#define myOled_NOP                                       0xE3

// Scrolling Commands
#define myOled_RIGHT_HORIZONTAL_SCROLL                            0x26
#define myOled_LEFT_HORIZONTAL_SCROLL                               0x27
#define myOled_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL    0x29
#define myOled_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL      0x2A
#define myOled_DEACTIVATE_SCROLL                                         0x2E
#define myOled_ACTIVATE_SCROLL                                             0x2F
#define myOled_SET_VERTICAL_SCROLL_AREA                            0xA3

// Addressing Setting Commands
#define myOled_SET_LOWER_COLUMN       0x00
#define myOled_SET_HIGHER_COLUMN      0x10
#define myOled_MEMORY_ADDR_MODE      0x20
#define myOled_SET_COLUMN_ADDR          0x21
#define myOled_SET_PAGE_ADDR               0x22

// Hardware Configuration Commands
#define myOled_SET_START_LINE              0x40
#define myOled_SET_SEGMENT_REMAP     0xA0
#define myOled_SET_MULTIPLEX_RATIO     0xA8
#define myOled_COM_SCAN_DIR_INC         0xC0
#define myOled_COM_SCAN_DIR_DEC        0xC8
#define myOled_SET_DISPLAY_OFFSET        0xD3
#define myOled_SET_COM_PINS                  0xDA
#define myOled_CHARGE_PUMP                  0x8D

// Timing & Driving Scheme Setting Commands
#define myOled_SET_DISPLAY_CLOCK_DIV_RATIO    0xD5
#define myOled_SET_PRECHARGE_PERIOD              0xD9
#define myOled_SET_VCOM_DESELECT                   0xDB

// I2C related
#define myOled_COMMAND               0x00
#define myOled_DATA                        0xC0
#define myOled_DATA_CONTINUE      0x40
#define myOled_ADDR                        0x3C  /**< I2C address alt 0x3D */ 

#define myOled_command(Reg)  I2C_Write_Byte(Reg, myOled_COMMAND)
#define myOled_data(Data)    I2C_Write_Byte(Data, myOled_DATA_CONTINUE)

// Pixel color
#define BLACK         0 
#define WHITE         1
#define INVERSE      2

// Delays
#define myOled_INITDELAY 100 /**< Initialisation delay in mS */


/*! 
	@brief class to control OLED and define buffer
*/
class myOled {
  public:
	myOled(int16_t oledwidth, int16_t oledheight);
	~myOled(){};

	uint8_t* buffer = nullptr;  /**< Buffer to hold screen data */

    void init(void);
	void update(void);
	void clearBuffer(void);

    void drawPixel(int16_t x, int16_t y, uint8_t color);
	
	void begin(uint16_t I2C_speed = 0, uint8_t I2c_address= myOled_ADDR );
	
    void setCursor(int16_t x, int16_t y);
    void drawChar(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size);

    size_t write(uint8_t);
    size_t write(const uint8_t *buffer, size_t size);
    size_t print(const std::string &s);

  private:

	void I2C_Write_Byte(uint8_t value, uint8_t Cmd);
	void I2C_OFF(void);
	void I2C_ON(void);
	
	uint16_t _I2C_speed = 0 ;             /**< Speed of I2C bus interface */
	uint8_t _I2C_address = myOled_ADDR ; /**< I2C address */
	
	int16_t _OLED_WIDTH;      /**< Width of OLED Screen in pixels */
	int16_t _OLED_HEIGHT;    /**< Height of OLED Screen in pixels */
	int8_t _OLED_PAGE_NUM; /**< Number of byte size pages OLED screen is divided into */ 
	uint8_t bufferWidth ;      /**< Width of Screen Buffer */ 
	uint8_t bufferHeight ;    /**< Height of Screen Buffer */

	const int16_t WIDTH;  /**< This is the 'raw' display w - never changes */
	const int16_t HEIGHT;  /**< This is the 'raw' display h - never changes*/

	int16_t _width;  /**< Display w as modified by current _rotation*/
	int16_t _height;  /**< Display h as modified by current _rotation*/
	int16_t _cursor_x; /**< Current X co-ord cursor position */
	int16_t _cursor_y;  /**< Current Y co-ord cursor position */
	uint8_t _rotation; /**< Current rotation 0-3 */
	
	uint8_t _textcolor = 1;  /**< Text foreground color */
	uint8_t _textbgcolor = 1;   /**< Text background color */
	uint8_t   _textsize = 1; /**< Size of text ,fonts 1-6 */
	bool _textwrap;                    /**< If set, '_textwrap' text at right edge of display*/

	uint8_t _FontNumber = 1;                /**< Holds current font number */
	uint8_t _CurrentFontWidth = 5;        /**<Holds current font width in bits */
	uint8_t _CurrentFontoffset = 0x00;  /**<Holds current font ASCII table offset */
	uint8_t _CurrentFontheight = 8;      /**<Holds current font height in bits */

    extern const unsigned char * pFontDefaultptr;

}; 
