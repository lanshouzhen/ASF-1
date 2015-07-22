/**
 * \file
 *
 * \brief SAM L22 SLCD Features Example Application
 *        
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \license
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
 /**
 * \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the SAM L22 Xplained pro SLCD example application 
 *
 * This example demonstrates SAM L22 how to use the SAM L22 SLCD.
 *
 * The supported board list:
 *    - SAM L22 Xplained Pro
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC and IAR Embedded Workbench
 * for Atmel. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * The SEGMENT LCD1 Xplained Pro extension board must be connected to SAM devices.
 *
 * \section configinfo Configuration Information
 * - PC terminal settings:
 *   - 115200 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com">Atmel</A>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>

#include <string.h>
#include <conf_slcd.h>
#include <conf_example.h>
		
uint16_t DIGI_LUT[] = {
// 0x npd0mkeclifbjh0ag
// 0x m0dnlcekjbfiga0h
	0b0000000000000000, //32 - Space
	0b0000000000000000, //33 - !
	0b0000000000000000, //34 - "
	0b0000000000000000, //35 - #
	0b0000000000000000, //36 - $
	0b0000000000000000, //37 - %
	0b0000000000000000, //38 - &
	0b0000000000000000, //39 - '
	0b0000000000000000, //40 - (
	0b0000000000000000, //41 - )
	0b1001100111011001, //42 - *
	0b1000000110000001, //43 - +
	0b0000000000000000, //44 - ,
	0b0000000100000000, //45 - -
	0b0000000000000000, //46 - .
	0b0000100000010000, //47 - /
	0b0010111001110100, //48 - 0
	0b0000010001000000, //49 - 1
	0b0010001111000100, //50 - 2
	0b0010010111000100, //51 - 3
	0b0000010111100000, //52 - 4
	0b0010010110100100, //53 - 5
	0b0010011110100100, //54 - 6
	0b0000010001000100, //55 - 7
	0b0010011111100100, //56 - 8
	0b0010010111100100, //57 - 9
	0b0000000000000000, //58 - :
	0b0000000000000000, //59 - ;
	0b0001000000010000, //60 - <
	0b0000000000000000, //61 - =
	0b0000100000001000, //62 - >
	0b0000000000000000, //63 - ?
	0b0000000000000000, //64 - @
	0b0000011111100100, //65 - A
	0b1010010101000101, //66 - B
	0b0010001000100100, //67 - C
	0b1010010001000101, //68 - D
	0b0010001110100100, //69 - E
	0b0000001110100100, //70 - F
	0b0010011100100100, //71 - G
	0b0000011111100000, //72 - H
	0b1010000000000101, //73 - I
	0b0010011001000000, //74 - J
	0b0001001010110000, //75 - K
	0b0010001000100000, //76 - L
	0b0000011001111000, //77 - M
	0b0001011001101000, //78 - N
	0b0010011001100100, //79 - O
	0b0000001111100100, //80 - P
	0b0011011001100100, //81 - Q
	0b0001001111100100, //82 - R
	0b0010010110100100, //83 - S
	0b1000000000000101, //84 - T
	0b0010011001100000, //85 - U
	0b0000101000110000, //86 - V
	0b0001111001100000, //87 - W
	0b0001100000011000, //88 - X
	0b1000000000011000, //89 - Y
	0b0010100000010100, //90 - Z			

};	
		
bool is_scrolling = false;		
bool is_blinking = false;
bool is_bitmapping = false;
uint8_t bitmapping_repeat_count = 0;

/* DMA and User scrolling string buffer length */
#define BUFFER_LEN 30

/** User scrolling string */
static char user_scrolling_str[BUFFER_LEN] = {"HELLO WORLD     "};
				
/* DMA resource and descriptor */
struct dma_resource example_resource;

/** Variables used for DMA transfer */						
static volatile uint32_t dma_source_buf[BUFFER_LEN];							
static volatile bool transfer_is_done = false;
static volatile uint32_t dma_break = 0, dma_block_count = 0;
				
COMPILER_ALIGNED(16)
DmacDescriptor example_descriptor;

/* Module instances */
static struct usart_module usart_instance;			

static void configure_dma_resource(struct dma_resource *resource)
{

	struct dma_resource_config config;
	/** Get the default configuration */
	dma_get_config_defaults(&config);
			
	if( true == is_scrolling ){
		/** Set the peripheral trigger source */
		config.peripheral_trigger = SLCD_DMAC_ID_ACMDRDY;
		/** Set the trigger action */
		config.trigger_action = DMA_TRIGGER_ACTON_BEAT;								
	}		
				
	if( true == is_bitmapping ){
		/** Set the peripheral trigger source */
		config.peripheral_trigger = SLCD_DMAC_ID_ABMDRDY;
		/** Set the trigger action */
		config.trigger_action = DMA_TRIGGER_ACTON_BEAT;
	}
						
	dma_allocate(resource, &config);

}

static void setup_transfer_descriptor(DmacDescriptor *descriptor )
{
	struct dma_descriptor_config descriptor_config;
					
	if( true == is_scrolling ){
		/** Get the default configuration */
		dma_descriptor_get_config_defaults(&descriptor_config);
				
		descriptor_config.beat_size = DMA_BEAT_SIZE_WORD;
		descriptor_config.src_increment_enable = true;
		descriptor_config.dst_increment_enable = false;	
		descriptor_config.step_size = (DMA_ADDRESS_INCREMENT_STEP_SIZE_1);
		descriptor_config.step_selection = DMA_STEPSEL_SRC;
		descriptor_config.block_action = DMA_BLOCK_ACTION_INT; 
				
		/** DMA transfer count found by Block count variable at 
		* run time from the scrolling string */
		descriptor_config.block_transfer_count = dma_block_count; 
		descriptor_config.source_address = (uint32_t)dma_source_buf + 
								(dma_block_count * sizeof(dma_source_buf[0]));
		descriptor_config.destination_address = (uint32_t)(&SLCD->CMDATA.reg); 
		descriptor_config.next_descriptor_address =( uint32_t )descriptor; 
				
		/** Create a DMA descriptor */
		dma_descriptor_create(descriptor, &descriptor_config);	
			
	}	
				
	if( true == is_bitmapping ){
		/** Get the default configuration */
		dma_descriptor_get_config_defaults(&descriptor_config);
			
		descriptor_config.beat_size = DMA_BEAT_SIZE_WORD; 
		descriptor_config.src_increment_enable = true;
		descriptor_config.dst_increment_enable = false;
		descriptor_config.step_size = (DMA_ADDRESS_INCREMENT_STEP_SIZE_1);
		descriptor_config.step_selection = DMA_STEPSEL_SRC;
		descriptor_config.block_action = DMA_BLOCK_ACTION_INT; 
				
		/** Block count manipulated number of the frame elements respective 
		* style chosen by the user at run time */
		descriptor_config.block_transfer_count = dma_block_count;
		descriptor_config.source_address = (uint32_t)dma_source_buf + 
								(dma_block_count * sizeof(dma_source_buf[0]));
		descriptor_config.destination_address = (uint32_t)(&SLCD->ISDATA.reg);
		descriptor_config.next_descriptor_address = ( uint32_t )descriptor;
				
		/** Create a DMA descriptor */
		dma_descriptor_create(descriptor, &descriptor_config);
			
	}	
				
}	
				
/**
* \brief Stop the SLCD1 automated character display mode.
*
* \note This function also disables the DMA channel associated with the
* SLCD1 module.
*/
static inline void slcd1_automated_char_stop(void)
{
	/* Disable automated character mode */
	slcd_disable_automated_character();	
	is_scrolling = false;
			
	/* Disable DMA channel */
	dma_free(&example_resource);		
						
}
		
/**
* \brief Stop the SLCD1 automated bit mapping display mode.
*
* \note This function also disables the DMA channel associated with the
* SLCD1 module.
*/
static inline void slcd1_automated_bit_stop(void)
{
	/* Disable automated bit mapping mode */
	slcd_disable_automated_bit();
	is_bitmapping = false;
			
	/* Disable DMA channel */
	dma_free(&example_resource);
						
}
		
/**
* \name Function Prototypes
* @{
*/
		
/**
* \brief Initialize the SLCD1 LCD Glass component.
*
* This function initializes the LCD driver to control the LCD glass.
* It perform LCD module initialization according to the SLCD1 characteristics.
*
*/
void slcd1_init(void);
		
/**
	* \brief Display the user menu on the terminal.
	*/
void display_menu(void);
		
/**
* \brief Show all content of the LCD glass.
*
* This function sets all pixels and areas of the LCD glass SLCD1.
*
*/
void slcd1_show_all(void);

/**
* \brief Clear all content of the LCD glass.
*
* This function clears all pixels and areas of the LCD glass SLCD1.
*
*/
void slcd1_clear_all(void);

/**
* \brief Show a specific icon on the LCD glass.
*
* This function will show a specific icon.
*
* \param icon_com  Pixel coordinate - COM - of the icon.
* \param icon_seg  Pixel coordinate - SEG - of the icon.
*
* \note Use the icon define in header file.
*/
void slcd1_show_icon(uint8_t icon_com, uint8_t icon_seg);	
		
/**
* \brief Clear a specific icon on the LCD glass.
*
* This function will clear a specific icon.
*
* \param icon_com  Pixel coordinate - COM - of the icon.
* \param icon_seg  Pixel coordinate - SEG - of the icon.
*
* \note Use the icon define in header file.
*/
void slcd1_clear_icon(uint8_t icon_com, uint8_t icon_seg);	
		 
/**
* \brief Blink a specific icon on the LCD glass.
*
* This function will blink a specific icon.
*
* \param icon_com  Pixel coordinate - COM - of the icon.
* \param icon_seg  Pixel coordinate - SEG - of the icon.
*
* \note Use the icon define in header file(with selected segments
* blinking feature).
*/
void slcd1_blink_icon_start(uint8_t icon_com, uint8_t icon_seg);

/**
* \brief Stop blink a specific icon on the LCD glass.
*
* This function will stop blink a specific icon.
*
* \param icon_com  Pixel coordinate - COM - of the icon.
* \param icon_seg  Pixel coordinate - SEG - of the icon.
*
* \note Use the icon define in header file(with selected segments
* blinking feature).
*/
void slcd1_blink_icon_stop(uint8_t icon_com, uint8_t icon_seg);

/**
* \brief Blink the current screen content.
*
* This function will make the current screen blink.
*
*/
void slcd1_blink_screen(void);

/**
* \brief Disable all Blink.
*
* This function will disable all Blink content.
*
*/
void slcd1_blink_disable(void);

/**
* \brief Set the SLCD1 LCD glass contrast.
*
*  This function allows to adjust the contrast of the SLCD1 LCD glass.
*
*  \param contrast 0 <= contrast value <= 15.
*/
void slcd1_set_contrast(uint8_t contrast);


/**
* \brief Show a decimal numeric value to LCD glass.
*
* This function displays an int32 value to the LCD numeric field of the glass.
*
* \param value The int32_t value to be displayed
*
* \note The value range is [-199999,199999].
*/
void slcd1_show_numeric_dec(int32_t value);

/**
* \brief Clear SLCD1 LCD glass numeric field.
*
* This function will clear the numeric field of the LCD glass.
*/
void slcd1_clear_numeric_dec(void);

/**
* \brief Scrolling start.
*
* This function start the text scrolling.
*
* \param data Data string buffer.
* \param length Data string length.
*/
void slcd1_text_scrolling_start(char *data);
		
/**
* \brief Scrolling start.
*
* This function start the Bit mapping text.
*
* \param data Data string buffer.
* \param length Data string length.
*/
void slcd1_text_bitmapping_start(uint8_t user_style);
		
/**
* \brief Scrolling stop.
*
* This function stop the auto bit mapping mode.
*/
void slcd1_text_bitmapping_stop(void);

/**
* \brief Scrolling stop.
*
* This function stop the text scrolling.
*/
void slcd1_text_scrolling_stop(void);		
		
/**
* \brief Show text on SLCD1 LCD glass alphanumeric field.
*
* This function will show text on the alphanumeric field of the LCD glass.
*
* \param data Pointer to the input string(max length is 7)
*/
void slcd1_show_text(const uint8_t *data);

/**
* \brief Clear SLCD1 LCD glass alphanumeric field.
*
* This function will clear the alphanumeric field of the LCD glass.
*/
void slcd1_clear_text(void);	
		
						
/**
* \brief DMA callback function for transfer complete.
*/
static void dma_callback(struct dma_resource* const resource )
{
	transfer_is_done = true;
	dma_break++;
	if( true == is_scrolling ){
		/** Used for automatically exit implementation */
		if((dma_block_count * 4) <= dma_break){
			dma_break = 0;
			/** Stop Automated character mode */
			slcd1_automated_char_stop();
			printf("Exits from scrolling mode.\n\n\r");
			display_menu();
		}
	}
				
	if( true == is_bitmapping ){	
		/** Used for automatically exit implementation */			
		if((bitmapping_repeat_count) <= dma_break){
			dma_break = 0;
			/** Stop Automated bit mode */
			slcd1_automated_bit_stop();
			printf("Exits from Bit mapping mode.\n\n\r");
			display_menu();
		}
	}
							
}	

/**
* \brief Config DMA to make peripheral-to-memory transfer scrolling string to 
* SLCD memory.
*/
static void configure_dma(void)
{
					
	configure_dma_resource(&example_resource);			
	setup_transfer_descriptor(&example_descriptor);			
							
	if(STATUS_OK != dma_add_descriptor(&example_resource, &example_descriptor)){
		printf("Descriptor status error\n\n\r");
	}

	dma_register_callback(&example_resource, dma_callback,
											DMA_CALLBACK_TRANSFER_DONE);			
	dma_enable_callback(&example_resource, DMA_CALLBACK_TRANSFER_DONE);					
			
}

/**
* \brief Configure serial console.
*/
static void configure_console(void)
{
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	config_usart.baudrate    = 9600;
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	stdio_serial_init(&usart_instance, EDBG_CDC_MODULE, &config_usart);
	usart_enable(&usart_instance);
			
}		
				
/**
* \brief Get the user string on the terminal.
*/
static void get_userstr(char * data, uint16_t length)
{
	do
	{
		printf(	"Type the user input string and press Enter\n\r");
				
		if (25 == length){
			printf("Note:-\n\r"
					"Max 25 characters "
					"(A-Z,0-9 and 'space' only).\n\n\r");
			scanf("%25[0-9A-Z ]s",data);
			strcat(data,"     ");
		}else {
			printf("Note:-\n\r"
					"Max 5 characters "
					"(A-Z,0-9 and 'space' only).\n\n\r");
			scanf("%5[0-9A-Z ]s",data);					
		}
				
		/*flushing stdio ....*/
		fflush(stdout);		
					
		printf("The input string entered is \"%s\" \n\n\r",data); //user_str
		printf("-----------------------------------------------\n\r");
		printf("Press 'e' to re-type or press any key to continue\n\n\r");
		getchar();
	} while (getchar() == 'e');			
					
			
}
		
/**
* \brief Clear the data buffers.
*/
static void clear_buffer(void)
{			
	uint32_t loop_count = 0;
	while((BUFFER_LEN - 1) >= loop_count)
	{				
		dma_source_buf[loop_count] = 0;	
							
		/** Clears the user scrolling string */
		if( false == is_bitmapping ){			
			user_scrolling_str[loop_count] = 0;
		}			
				
		loop_count++;
	}
}
		
/**
* \brief Display the user menu on the terminal.
*/
void display_menu(void)
{
	printf("\n\r");
	printf("===============================================\n\r");
	printf("Menu: Select the option.\n\r");
	printf("===============================================\n\r");
			
	printf("  1 : Display UART user input string in SLCD \n\r");
	printf("  2 : Character mapping feature \n\r");
	printf("  3 : Auto Character mapping feature \n\r");
	printf("  4 : Blinking feature \n\r");
	printf("  5 : Auto Bit mapping feature \n\n\r");
			
	printf("  c : Clear the SLCD screen \n\r");
	printf("  e : To change the user scrolling string input \n\r");

	printf("-----------------------------------------------\n\r");
	printf(">>");
			
}
		
/**
* \name Function Definitions
* @{
*/

void slcd1_init(void)
{
	struct slcd_config config;

	slcd_get_config_defaults(&config);
	config.waveform_mode = SLCD_LOW_POWER_WAVEFORM_MODE;
														
	slcd_init(&config);

	slcd_set_frame_counter(SLCD_FRAME_COUNTER_0,false,CONF_SLCD1_FC0);
	slcd_set_frame_counter(SLCD_FRAME_COUNTER_1,false,CONF_SLCD1_FC1);
	slcd_set_frame_counter(SLCD_FRAME_COUNTER_2,false,CONF_SLCD1_FC2);
	slcd_set_contrast(CONF_SLCD1_CONTRAST);
	
}
				
void slcd1_show_all(void)
{
	slcd_disable();
	slcd_disable_blink();
	slcd_disable_circular_shift();
	slcd_set_display_memory();
	slcd_enable();
}

void slcd1_clear_all(void)
{
	slcd_disable_blink();
	slcd_disable_circular_shift();
	slcd_clear_display_memory();
	slcd1_text_scrolling_stop();
	slcd1_text_bitmapping_stop();
	dma_break = 0;
}

void slcd1_show_icon(uint8_t icon_com, uint8_t icon_seg)
{
	slcd_set_pixel(icon_com, icon_seg);
}
		
void slcd1_clear_icon(uint8_t icon_com, uint8_t icon_seg)
{
	slcd_clear_pixel(icon_com, icon_seg);
}	
		
void slcd1_blink_icon_start(uint8_t icon_com, uint8_t icon_seg)
{
	if (icon_seg < 2) {
		slcd1_show_icon(icon_com,icon_seg);
		slcd_disable();
		struct slcd_blink_config blink_config;
		slcd_blink_get_config_defaults(&blink_config);
		blink_config.blink_all_seg = false;
		blink_config.fc = CONF_SLCD1_BLINK_TIMER;
		slcd_blink_set_config(&blink_config);
		slcd_set_blink_pixel(icon_com, icon_seg);
		slcd_enable_frame_counter(CONF_SLCD1_BLINK_TIMER);
		slcd_enable_blink();
		slcd_enable();
	}

}

void slcd1_blink_icon_stop(uint8_t icon_com, uint8_t icon_seg)
{
	if (icon_seg < 2) {
		slcd_disable_blink();
		slcd_disable();
		slcd_clear_blink_pixel(icon_com, icon_seg);
		slcd_enable_blink();
		slcd_enable();
	}
}

void slcd1_blink_screen(void)
{
	slcd_disable_blink();
	slcd_disable();
	struct slcd_blink_config blink_config;
	slcd_blink_get_config_defaults(&blink_config);
	blink_config.blink_all_seg = true;
	blink_config.fc = CONF_SLCD1_BLINK_TIMER;
	slcd_blink_set_config(&blink_config);

	slcd_enable_frame_counter(CONF_SLCD1_BLINK_TIMER);
	slcd_enable_blink();
	slcd_enable();
}

void slcd1_blink_disable(void)
{
	slcd_disable_frame_counter(CONF_SLCD1_BLINK_TIMER);
	slcd_disable_blink();
}

void slcd1_set_contrast(uint8_t contrast)
{
	slcd_set_contrast(contrast); //max 0-f
}

void slcd1_show_numeric_dec(int32_t value)
{
	uint32_t tmp=0;
	Assert(value > -200000);
	Assert(value < 200000);

	slcd_character_map_set(SLCD_AUTOMATED_CHAR_START_FROM_BOTTOM_RIGHT,3);

	if(value < 0) {
		slcd_set_pixel(SLCD1_ICON_MINUS);
		} else {
		slcd_clear_pixel(SLCD1_ICON_MINUS);
	}

	tmp = Abs(value);

	if (tmp > 99999) {
		slcd_set_pixel(SLCD1_ICON_MINUS_SEG1);
		slcd_set_pixel(SLCD1_ICON_MINUS_SEG2);
		tmp -= 100000;
		} else {
		slcd_clear_pixel(SLCD1_ICON_MINUS_SEG1);
		slcd_clear_pixel(SLCD1_ICON_MINUS_SEG2);
	}

	for(int32_t i=SLCD1_MAX_NUM-1; (i >= 0); i--){
		slcd_character_write_data(0,(SLCD1_NUM_SEG_INDEX_S+i*4),
						DIGI_LUT[(tmp%10) +15],0x4002);			
		tmp /=10;				
					
	}

}

void slcd1_clear_numeric_dec(void)
{
	slcd_clear_pixel(SLCD1_ICON_MINUS);
	slcd_clear_pixel(SLCD1_ICON_MINUS_SEG1);
	slcd_clear_pixel(SLCD1_ICON_MINUS_SEG2);

	slcd_clear_pixel(SLCD1_ICON_DOT_1);
	slcd_clear_pixel(SLCD1_ICON_DOT_2);
	slcd_clear_pixel(SLCD1_ICON_DOT_3);
	slcd_clear_pixel(SLCD1_ICON_DOT_4);

	slcd_clear_pixel(SLCD1_ICON_AM);
	slcd_clear_pixel(SLCD1_ICON_PM);
	slcd_clear_pixel(SLCD1_ICON_DEGREE_C);
	slcd_clear_pixel(SLCD1_ICON_DEGREE_F);
	slcd_clear_pixel(SLCD1_ICON_VOLT);
	slcd_clear_pixel(SLCD1_ICON_MILLI_VOLT);
	slcd_character_map_set(SLCD_AUTOMATED_CHAR_START_FROM_BOTTOM_RIGHT,3);
	for(uint32_t i = 0 ; i < SLCD1_MAX_NUM ; i++) {
		slcd_character_write_data(0,SLCD1_NUM_SEG_INDEX_S+i*4,
		0,0x4002);
	}
}

void slcd1_text_scrolling_start(char *data)
{
	/* Structures for the ACM configurations */
	struct slcd_automated_char_config automated_char_config;
				
	uint16_t i=0;
	dma_block_count = 0;
						
	/** used to copy the user scrolling string into dma_source
	* buffer for the DMA transfer 
	* The scrolling may be runtime user input string 25char max
	* or the default scrolling string in user_scrolling_str[]
	*/
	while (*data != '\0'){
		/** Copying the scrolling string into DMA source buffer*/
		dma_source_buf[i++] = DIGI_LUT[*(data++) - 32];
		/** To find the scrolling string length */
		dma_block_count++; 
	}
					
	if( true == is_scrolling ){
		/** configuration for Auto character mapping scrolling mode */
		/** Get default config for the ACM mode */
		slcd_automated_char_get_config_default(&automated_char_config);
		/** Segment mapping order (CMCFG.DEC bit) */ 
		automated_char_config.order = SLCD_AUTOMATED_CHAR_START_FROM_BOTTOM_RIGHT;
		/** select the number of segment used per digit
		* it equal to number of SEG line - 1 (CMCFG.DEC) */
		automated_char_config.seg_line_num = 3;
				
		/** Define the number of digit per row. (ACMCFG.NDROW) */
		automated_char_config.row_digit_num = SLCD1_MAX_CHAR;
		/** Define the index of the first segment terminal of the 
		* digit to display (ACMCFG.STSEG) */
		automated_char_config.start_seg_line = SLCD1_TXT_SEG_INDEX_S;
		/** Define the number of digit, it must be greater than 1.
		* (ACMCFG.NDIG)*/
		automated_char_config.digit_num = SLCD1_MAX_CHAR;
				
		/** STEPS = string length - digit_num + 1 (ACMCFG.STEPS) */
		automated_char_config.scrolling_step = dma_block_count - 5 + 1;
		/** Select the ACM mode (ACMCFG.MODE) */				
		automated_char_config.mode  = SLCD_AUTOMATED_CHAR_SCROLL;
		/** Select the frame counter for the ACM mode (ACMCFG.FCS) */
		automated_char_config.fc = SLCD_FRAME_COUNTER_1;		
				
		/** Configure the mask value in the CMDMASK register  */		
		automated_char_config.data_mask = 0x00FF4002;
			
	}		
			
	/** Disable SLCD to write the enable-protected registers */
	slcd_disable();
	/** write the SLCD ACM configurations in the respective registers */
	slcd_automated_char_set_config(&automated_char_config);			
			
	/** Disable Frame counter to write the enable-protected 
	* registers */	
	slcd_disable_frame_counter(automated_char_config.fc);
	slcd_dma_display_memory_update_fc_sel(automated_char_config.fc);		
				
	/** Set the frame counter configurations and enable it */
	slcd_set_frame_counter(automated_char_config.fc,0,0x1); 
	slcd_enable_frame_counter(automated_char_config.fc);
				
	/** Enable SLCD */
	slcd_enable();			
					
	/** Configure DMA */			
	configure_dma();
			
	/** Start DMA transfer once DMA gets the peripheral 
	* trigger form the ACM, the data transfer starts */			
	dma_start_transfer_job(&example_resource);	
			
	/** Enable ACM mode */
	slcd_enable_automated_character();		
				
}
		
void slcd1_text_scrolling_stop(void)
{
	slcd1_automated_char_stop();
}		
		
void slcd1_text_bitmapping_start(uint8_t user_style)
{
	/** Default config for automated bit mapping */
	uint8_t config_user_style = (user_style & 0x03); 
	uint8_t config_size = 0;
	uint8_t config_fc_value = 0;
	enum slcd_frame_counter config_fc = SLCD_FRAME_COUNTER_0;
				
	/** User buffer to load the user style in this array 
	* new styles can be added as a new with same column width. 
	* At the end of the each style array member value should 
	* be kept as 0x0000FF00 to calculate the block_count.
	* The empty elements between the style members should 
	* be 0x0000FFFF*/						
	uint16_t i=0;	
	uint32_t user_style_buf[5][30] = { 
		{ 
			0x00000040,0x000600e0,0x000C0070,0x00120000, // S // 'A' // 0x07e4
			0x00010005,0x00070000,0x000D0000,0x00130008, // T // 'T' // 0x8005
			0x00010085,0x00070070,0x000D0060,0x00130008, // Y // 'M' // 0x0678
			0x00020004,0x0008000a,0x000E0003,0x00140002, // L // 'E' // 0x23a4
			0x00020004,0x0008002a,0x000E0023,0x00140022, // E // 'L' // 0x2220
			0x0000FF00,0x0000FF00,0x0000FF00,0x0000FF00  // 0
		},
		{
			0x00000040,0x00070000,0x000D0060,0x00140002, // S
			0x000600e0,0x000D0060,0x00130008,0x00020004, // T
			0x000C0070,0x00130008,0x00020004,0x0008000a, // Y
			0x00120000,0x00010005,0x0008002a,0x000E0003, // L
			0x00010085,0x00070070,0x000E0023,0x00140022, // E
			0x0000FF00,0x0000FF00,0x0000FF00,0x0000FF00  //	1				
		},
		{
			0x00000040,0x00010004,0x00020004,0x00010045, // S
			0x00060060,0x00070070,0x00010085,0x000600E0, // T
			0x00080022,0x0008002a,0x000C0010,0x000E0001, // Y
			0x000C0070,0x000D0060,0x000E0023,0x00130008, // L
			0x00140022,0x0000FF00,0x0000FF00,0x0000FF00, // E
			0x0000FF00,0x0000FF00,0x0000FF00,0x0000FF00  // 2					
		},
		{
			0x00000040,0x00010044,0x0000FFFF,0x0000FFFF, // S
			0x00020044,0x00080040,0x00000000,0x00010000, // T
			0x000E0040,0x00140022,0x00020000,0x00080000, // Y
			0x00120020,0x00130022,0x000E0000,0x00140000, // L
			0x000C0020,0x00060020,0x00120000,0x00130000, // E
			0x00000040,0x000C0000,0x00060000,0x0000FF00, // 3
		},
		{
			0x00000040,0x00010004,0x00020004,0x00010045, // S
			0x00060060,0x00070070,0x00010085,0x000600E0, // P
			0x00080022,0x0008002a,0x000C0010,0x000E0001, // A
			0x000C0070,0x000D0060,0x000E0023,0x00130008, // R
			0x00140022,0x0000FF00,0x0000FF00,0x0000FF00, // E
			0x0000FF00,0x0000FF00,0x0000FF00,0x0000FF00  // x
		}
				
								
	};
			
	/** Variable use to find the DMA transfer count 
	* runtime based on the user input string/style*/	
	dma_block_count = 0;		
			
	/** Clears the previous values present in the dma_source_buf */
	clear_buffer();
			
	/** used to copy the user style to dma_source buffer 
	* for the DMA transfer. 
	* The user style options are Style0, Style1, Style2 & Style3
	*/			
	while ( 0x0000FF00 !=  user_style_buf[config_user_style][i] ){
		dma_source_buf[i] = user_style_buf[config_user_style][i];
		/** To find the user style number of segment data */
		dma_block_count++;
		i++;
	}
						
	/** user configurations for automated bit mapping */
			
	if((uint8_t) 0x03 == config_user_style ){
		config_size = 1;
		config_fc_value = 0x01;
		bitmapping_repeat_count = 0x05;
	}else {
		config_size = 3;
		config_fc_value = 0x0F; 
		bitmapping_repeat_count = 0x01;
	}			
	config_fc = SLCD_FRAME_COUNTER_1;
			
	/** Disable SLCD to write the enable-protected registers */					
	slcd_disable();
	/** write the SLCD ABM configurations in the respective registers */
	slcd_set_automated_bit(config_size, config_fc);
			
	/** Disable Frame counter to write the enable-protected 
	* registers */	
	slcd_disable_frame_counter(config_fc);			
	
	/** Set the frame counter configurations and enable it */
	slcd_set_frame_counter(config_fc,1,config_fc_value); 	
	slcd_enable_frame_counter(config_fc);
			
	/** Enable SLCD */			
	slcd_enable();			
						
	/** Configure DMA */
	configure_dma();
			
	/** Start DMA transfer once DMA gets the peripheral 
	* trigger form the ABM, the data transfer starts */				
	dma_start_transfer_job(&example_resource);
			
	/** Enable ACM mode */
	slcd_enable_automated_bit();		
			
}
		
void slcd1_text_bitmapping_stop(void)
{
	slcd1_automated_bit_stop();
}		
		
void slcd1_show_text(const uint8_t *data)
{
	Assert(data);
	uint32_t len = (uint32_t)strlen((char *) data);
			
	len = (len > SLCD1_MAX_CHAR) ? SLCD1_MAX_CHAR : len;
			
	slcd_character_map_set(SLCD_AUTOMATED_CHAR_START_FROM_BOTTOM_RIGHT,3);
	for(uint32_t i = 0 ; *data != '\0' ; i++) {
			slcd_character_write_data(0,SLCD1_TXT_SEG_INDEX_S+i*4,
			DIGI_LUT[*(data++) - 32],0x4002); 										
	}					

}

void slcd1_clear_text(void)
{
	slcd_character_map_set(SLCD_AUTOMATED_CHAR_START_FROM_BOTTOM_RIGHT,3);
	for(uint32_t i = 0 ; i < SLCD1_MAX_CHAR ; i++) {
		slcd_character_write_data(0,SLCD1_TXT_SEG_INDEX_S+i*4,
		0,0x4002);
	}
}


int main (void)
{
	uint8_t key = 0;
	/** Temporary buffer used for get the user input string */	
	char user_str[]={""};			
						
	/** Initialize the SAM system */
	system_init();
				
	//Set NVM wait state
	NVMCTRL->CTRLB.bit.RWS = 3;
			
	board_init();
					
	/** Initialize the console USART */
	configure_console();
						
	/** Output example information */
	printf("\n\r");
	printf("===============================================\n\r");
	printf("	     -- SLCD Example --				   \n\r");
	printf("===============================================\n\n\r");
	printf("-- %s\n\r", BOARD_NAME);
	printf("-- Compiled: %s %s --\n\n\r", __DATE__, __TIME__);
			
	printf("-----------------------------------------------\n\r");
	printf("\n\r");
			
	/** Turn on the backlight. */
	//ioport_set_pin_level(LCD_BL_GPIO, LCD_BL_ACTIVE_LEVEL);
			
	/** Initialize the SLCD1 LCD glass component. */	
	slcd1_init();
	
	/** Enable SLCD */		
	slcd_enable();
			
	/** Enables all the segments in the SLCD */
	printf("Printing all of the SLCD segments.\n\n\r");
	slcd1_show_all();
			
	printf("Press any key to continue.\n\n\r");
	scanf("%c", (char *)&key);
	
	/** Clear the SLCD and disables all the running modes */
	slcd1_clear_all();			
			
	/** Display menu */
	display_menu();
			
	while(1){
		/** Get the user option */	
		scanf("%c", (char *)&key);
				
		switch (key) {
			case 'c':
			case 'C':
				printf("c: Clear the SLCD screen \n\n\r");
				printf("Clearing... \n\r");
				slcd1_clear_all();
				display_menu();	
				break;
						
			case 'e':
			case 'E':
				printf("e: To change the user scrolling string input \n\n\r");
				slcd1_clear_all();
				clear_buffer();
				get_userstr(user_str,25);									
				strcpy(user_scrolling_str,user_str);
				display_menu();
				break;					

			case '1':
				printf("1: Display UART user input string in SLCD \n\n\r");						
				get_userstr(user_str,5);						
				slcd1_clear_all();
				printf("Printing -- [ %s ] \n\r",user_str);
				slcd1_show_text((const uint8_t *)user_str);		
				display_menu();
				break;

			case '2':
				slcd1_clear_all();
				printf("2: Character mapping feature.\n\n\r");
				printf("Running... \n\r");
				slcd1_show_text((const uint8_t *)"ATMEL");
				delay_s(2);
						
				slcd1_clear_all();
				/** values range -199999 to 199999 */
				slcd1_show_numeric_dec(-123456); 
				delay_s(2);
				display_menu();						
				break;
						
			case '3':
				slcd1_clear_all();
				printf("3: Auto Character mapping feature.\n\n\r  "
						"Text scrolling...                        " 
						"\n\n\r");
				is_scrolling = true;							
				slcd1_text_scrolling_start(user_scrolling_str);						
				printf("Press 'c' to exit scrolling and clear display. \n\r"
						"Otherwise it will exit automatically\n\n\r"); 
				slcd1_show_icon(SLCD1_ICON_ATMEL);	
				printf("-----------------------------------------------\n\r");
				break;
						
			case '4':
				slcd1_clear_all();
				printf("4: Blinking feature.\n\n\r");
				printf("Blink screen...\n\r");				
				slcd1_show_text((const uint8_t *)"BLINK");
				slcd1_blink_screen();
				delay_s(2);					
				display_menu();
				break;
							
			case '5':
				slcd1_clear_all();
				printf("5: Auto Bit mapping feature.\n\n\r"
						"Select the user style\n\r"
						"Press 0 for Style0\n\r"
						"Press 1 for Style1\n\r"
						"Press 2 for Style2\n\r"
						"Press 3 for Style3\n\r" 
						"\n\r");
				is_bitmapping = true;
				scanf("%c", (char *)&key);
				switch (key){
					case '0': 
					case '1':
					case '2':
					case '3': 
						printf("Style%c selected.\n\n\r",key);						
						slcd1_text_bitmapping_start(key);							
						printf("Press 'c' to exit Auto bit mapping and" 
						"clear display. \n\r"
						"Otherwise it will exit automatically\n\n\r");
						break;
					default: 
						printf("Invalid input received.\n\n\r");
						display_menu();			
						break;	
				}						
							
				break;
						  						
			default:
				break;
		}//switch
				
				
	}//while
	
}		
