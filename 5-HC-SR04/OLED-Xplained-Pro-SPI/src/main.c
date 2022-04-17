#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

#define LED_PIO   PIOC
#define LED_PIO_ID    ID_PIOC
#define LED_PIO_IDX   8
#define LED_IDX_MASK (1u << LED_PIO_IDX)

#define ECHO_PIO      PIOA
#define ECHO_PIO_ID   ID_PIOA
#define ECHO_IDX  6
#define ECHO_IDX_MASK (1 <<ECHO_IDX)


volatile uint32_t timer;


void but_callback(void);

void pisca_led(int n, int t);
void io_init(void);

void pin_toggle(Pio *pio, uint32_t mask) {
	pio_get_output_data_status(pio, mask) ? pio_clear(pio, mask) : pio_set(pio,mask);
}


static float get_time_rtt(){
	uint ul_previous_time = rtt_read_timer_value(RTT);
}

static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource) {

	uint16_t pllPreScale = (int) (((float) 32768) / freqPrescale);
	
	rtt_sel_source(RTT, false);
	rtt_init(RTT, pllPreScale);
	
	if (rttIRQSource & RTT_MR_ALMIEN) {
		uint32_t ul_previous_time;
		ul_previous_time = rtt_read_timer_value(RTT);
		while (ul_previous_time == rtt_read_timer_value(RTT));
		rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);
	}

	/* config NVIC */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 4);
	NVIC_EnableIRQ(RTT_IRQn);

	/* Enable RTT interrupt */
	if (rttIRQSource & (RTT_MR_RTTINCIEN | RTT_MR_ALMIEN))
	rtt_enable_interrupt(RTT, rttIRQSource);
	else
	rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_ALMIEN);
	
}

void RTT_Handler(void) {
	uint32_t ul_status;

	ul_status = rtt_get_status(RTT);

}


void echo_callback(void){
	if(pio_get(ECHO_PIO,PIO_INPUT,ECHO_IDX_MASK)){
		RTT_init(8620,(int) 8620*(4/340) ,0);
	}
	
	else{
		timer=rtt_read_timer_value(RTT);
	}
}
void io_init(void)
{
	
	
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_configure(LED_PIO, PIO_OUTPUT_1, LED_IDX_MASK, PIO_DEFAULT);
	

	pmc_enable_periph_clk(ECHO_PIO_ID);
	pio_configure(ECHO_PIO, PIO_INPUT, ECHO_IDX_MASK, PIO_PULLUP);
	pio_set_debounce_filter(ECHO_PIO, ECHO_IDX_MASK, 60);
	pio_get_interrupt_status(ECHO_PIO);

	pio_enable_interrupt(ECHO_PIO, ECHO_IDX_MASK);

	pio_handler_set(ECHO_PIO,
					ECHO_PIO_ID,
					ECHO_IDX_MASK,
					PIO_IT_EDGE,
					echo_callback);

	NVIC_EnableIRQ(ECHO_PIO_ID);
	NVIC_SetPriority(ECHO_PIO_ID, 4);
}

int main (void)
{
	board_init();
	sysclk_init();
	delay_init();

	// Init OLED
	gfx_mono_ssd1306_init();
	io_init();
	
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	RTT_init(4, 16, 0);

	char string[50];

	/* Insert application code here, after the board has been initialized. */
	while(1) {
		
		//pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
		
		pio_set(LED_PIO,LED_IDX_MASK);
		delay_us(10);
		pio_clear(LED_PIO,LED_IDX_MASK);
		
		double f = 8620.0;
		
		double time=timer/f;
		
		double distance = (340*time/2) * 100;
		
		sprintf(string, "%f cm", distance);
		gfx_mono_draw_string(string, 0, 0, &sysfont);

	}
}