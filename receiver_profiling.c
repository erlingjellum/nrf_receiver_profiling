#include "nrf.h"
#include "receiver_profiling.h"

void receiver_profiling_init(void)
{
	//Initialize the parameters for choosing gpiote-output, pin and which ppi-channels to use
	uint8_t gpiote_output_channel = 3;
	uint32_t gpio_output_pin = 30;
	
	uint8_t ppi_channel_1 = 12;
	uint8_t ppi_channel_2 = 13;
	uint8_t ppi_channel_3 = 14;
	uint8_t ppi_channel_4 = 15;
	
	receiver_profiling_gpiote_setup(gpiote_output_channel, gpio_output_pin);
	receiver_profiling_ppi_setup(ppi_channel_1, ppi_channel_2, ppi_channel_3, ppi_channel_4, gpiote_output_channel);
	
}


void receiver_profiling_gpiote_setup(uint8_t gpiote_output_channel, uint32_t gpio_output_pin)
{
	NRF_GPIOTE->CONFIG[gpiote_output_channel] = ((GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos) & GPIOTE_CONFIG_MODE_Msk)
												|	((gpio_output_pin << GPIOTE_CONFIG_PSEL_Pos) & GPIOTE_CONFIG_PSEL_Msk)
												|	((GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos) & GPIOTE_CONFIG_POLARITY_Msk)	
												|	((GPIOTE_CONFIG_OUTINIT_High << GPIOTE_CONFIG_OUTINIT_Pos) & GPIOTE_CONFIG_OUTINIT_Msk);
}


void receiver_profiling_ppi_setup(uint8_t ppi_channel_1, uint8_t ppi_channel_2, uint8_t ppi_channel_3,uint8_t ppi_channel_4, uint8_t gpiote_output_channel)
{
	NRF_PPI->CHENSET = ((1UL << ppi_channel_1) | (1UL << ppi_channel_2) | (1UL << ppi_channel_3) | (1UL << ppi_channel_4));
	
	NRF_PPI->CH[ppi_channel_1].EEP = (uint32_t) &NRF_RADIO->EVENTS_READY;
	NRF_PPI->CH[ppi_channel_1].TEP = (uint32_t) &NRF_GPIOTE->TASKS_OUT[gpiote_output_channel];
	
	NRF_PPI->CH[ppi_channel_2].EEP = (uint32_t) &NRF_RADIO->EVENTS_PAYLOAD;
	NRF_PPI->CH[ppi_channel_2].TEP	= (uint32_t) &NRF_GPIOTE->TASKS_OUT[gpiote_output_channel];
	
	NRF_PPI->CH[ppi_channel_3].EEP = (uint32_t) &NRF_RADIO->EVENTS_END;
	NRF_PPI->CH[ppi_channel_3].TEP = (uint32_t) &NRF_GPIOTE->TASKS_OUT[gpiote_output_channel];
	
	NRF_PPI->CH[ppi_channel_4].EEP = (uint32_t) &NRF_RADIO->EVENTS_ADDRESS;
	NRF_PPI->CH[ppi_channel_4].TEP = (uint32_t) &NRF_GPIOTE->TASKS_OUT[gpiote_output_channel];
}


