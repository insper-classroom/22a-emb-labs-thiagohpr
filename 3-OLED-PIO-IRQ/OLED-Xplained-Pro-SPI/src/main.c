#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

#define LED_PIO           PIOC
#define LED_PIO_ID        ID_PIOC
#define LED_PIO_IDX       8
#define LED_PIO_IDX_MASK  (1 << LED_PIO_IDX)

#define BUT1_PIO           PIOD
#define BUT1_PIO_ID        ID_PIOD
#define BUT1_PIO_IDX       28
#define BUT1_PIO_IDX_MASK (1u << BUT1_PIO_IDX)

#define BUT2_PIO           PIOC
#define BUT2_PIO_ID        ID_PIOC
#define BUT2_PIO_IDX       31
#define BUT2_PIO_IDX_MASK (1u << BUT2_PIO_IDX)

#define BUT3_PIO           PIOA
#define BUT3_PIO_ID        ID_PIOA
#define BUT3_PIO_IDX       19
#define BUT3_PIO_IDX_MASK (1u << BUT3_PIO_IDX)

volatile char but_flag;
volatile char but2_flag;
volatile char but3_flag;


void but_callback (void) {
	if (!pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK)){
		but_flag=1;
	}
	else{
		but_flag=0;
	}
	
}

void but2_callback (void) {
	if (!but2_flag){
		but2_flag=1;
		}
	else{
		but2_flag=0;
	}
	
}
	
void but3_callback (void) {
	but3_flag=1;
}
		
void pisca_led(int t_ms){
	  pio_clear(LED_PIO, LED_PIO_IDX_MASK);
	  delay_ms(t_ms);
	  pio_set(LED_PIO, LED_PIO_IDX_MASK);
	  delay_ms(t_ms);


}


void init (void){
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);
	
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pio_set_input(BUT1_PIO, BUT1_PIO_IDX_MASK, PIO_DEFAULT);
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT1_PIO, BUT1_PIO_IDX_MASK, 60);

	
	pio_handler_set(BUT1_PIO,
					BUT1_PIO_ID,
					BUT1_PIO_IDX_MASK,
					PIO_IT_EDGE,
					but_callback);
	pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT1_PIO);
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 4);
	
	
	
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pio_set_input(BUT2_PIO, BUT2_PIO_IDX_MASK, PIO_DEFAULT);
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT2_PIO, BUT2_PIO_IDX_MASK, 60);

	
	pio_handler_set(BUT2_PIO,
					BUT2_PIO_ID,
					BUT2_PIO_IDX_MASK,
					PIO_IT_RISE_EDGE,
					but2_callback);
	pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT2_PIO);
	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_SetPriority(BUT2_PIO_ID, 4);
	
	
	
	pmc_enable_periph_clk(BUT3_PIO_ID);
	pio_set_input(BUT3_PIO, BUT3_PIO_IDX_MASK, PIO_DEFAULT);
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT3_PIO, BUT3_PIO_IDX_MASK, 60);

	
	pio_handler_set(BUT3_PIO,
					BUT3_PIO_ID,
					BUT3_PIO_IDX_MASK,
					PIO_IT_RISE_EDGE,
					but3_callback);
	pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT3_PIO);
	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT3_PIO_ID, 4);
}

int main (void)
{
	board_init();
	sysclk_init();
	delay_init();
	init();

	int delay=500;
  // Init OLED
	gfx_mono_ssd1306_init();
  
  // Escreve na tela um circulo e um texto
	//gfx_mono_draw_filled_circle(20, 16, 16, GFX_PIXEL_SET, GFX_WHOLE);
	gfx_mono_draw_rect(10,5,110,10,GFX_PIXEL_SET);
	gfx_mono_draw_filled_rect(10, 5, 11*(delay/100), 10, GFX_PIXEL_SET);
	char str[128];
	sprintf(str, "%d", delay);
	gfx_mono_draw_string(str, 50,16, &sysfont);

  /* Insert application code here, after the board has been initialized. */
	while(1) {
		
		if(but_flag){
			delay_ms(700);
			if (but_flag){
				delay+=100;
				if (delay>=1000){
					delay=1000;
				}
				gfx_mono_draw_filled_rect(10, 5, 11*(delay/100), 10, GFX_PIXEL_SET);
			}
			else{
				delay-=100;
				if (delay<0){
					delay=0;
				}
				gfx_mono_draw_filled_rect(10, 5, 110, 10, GFX_PIXEL_CLR);
				gfx_mono_draw_rect(10,5,110,10,GFX_PIXEL_SET);
				gfx_mono_draw_filled_rect(10, 5, 11*(delay/100), 10, GFX_PIXEL_SET);
				
			}
			gfx_mono_draw_string("    ", 50,16, &sysfont);
			sprintf(str, "%d", delay);
			gfx_mono_draw_string(str, 50,16, &sysfont);
			but_flag=0;
			
		}
		if (but3_flag){
			delay-=100;
			gfx_mono_draw_filled_rect(10, 5, 110, 10, GFX_PIXEL_CLR);
			gfx_mono_draw_rect(10,5,110,10,GFX_PIXEL_SET);
			gfx_mono_draw_filled_rect(10, 5, 11*(delay/100), 10, GFX_PIXEL_SET);
			gfx_mono_draw_string("    ", 50,16, &sysfont);
			sprintf(str, "%d", delay);
			gfx_mono_draw_string(str, 50,16, &sysfont);
			but3_flag=0;
		}
		if (!but2_flag){
			if (delay!=0){
				pisca_led(delay);
			}
			
		}
		
	}
}

