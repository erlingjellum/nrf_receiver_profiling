/* The purpose of this module is to toggle pins on the nRF51 at certain events so that we can measure
how the receiver spends its time with a logical analyzer.

QUICK START:
Enable this module by calling "receiver_profiling_init() at the end of the initializion of the RADIO in main


REQUIREMENTS: 
* This .c file and it's .h file must lie within the project and the nrf.h must be on 
the include path.
* Must have at least 1 available GPIOTE TASK/EVENT Register. We will use OUT[3] as default
* At least 3 available PPI-channels. We will use CH13, CH14, CH15 by default
* At least 1 available GPIO PIN. Default is PIN30


ASSUMPTIONS ABOUT YOUR SYSTEM:
We have made the following assumptions about the design of your radio:
* Shortcut END->DISABLED enabled
* Shortcut RXEN->START enabled
* RX_EN is called during init. Thus the Receiver is in RX state when the test start (the first pin toggle corresponds to an ADDRESS event)
* The flow of tasks/events of the receiver is assumed to be:

RADIO_RXEN (task) -> READY (event) -> (shortcut) -> RX(state) -> ADDRESS(event) -> PAYLOAD(event)
-> END(event) -> (shortcut) -> DISABLED. # PROCESSING OF PACKAGE # -> RADIO_RXEN(task)


HOW-IT-WORKS
We generate PIN-toggles on PIN30 at the following events: READY, PAYLOAD, END. We assume that the system is in state RX
when we start the test. The output on PIN30 should be as following for when receiving ONE single packet:

	
				ADDRESS										PAYLOAD   END	  					 READY
					|____________________________|		 |_________________|
					|														 |	   |		  					 |
					|														 |		 |								 |
__________|														 |_____|								 |____________________ 	
						
----------|----------------------------|-----|-----------------|----------------
					|		 RECEIVE TIME					|OVERHEAD|PACKET PROCESSING|
					|													|			 	 |	+ RAMP UP      |

We will provide a Python3 script that will automate the reading and caluclation with a SALAE LOGIC ANALYZER

COMMON PITFALLS:
* GPIOTE[4], PPI[13], PPI[14], PPI[15] or PIN30 is already in use. 
SOLUTION: Change the relevant values in this .c file
* configuration of PPI or GPIOTE elsewhere in your project is overwriting the configuration made in this 
SOLUTION: Use the '|=' rather than the '=' operator to configurate the registers in your code elsewhere.
*/


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


