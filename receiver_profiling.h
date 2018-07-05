#include <stdint.h>

void receiver_profiling_init(void);

void receiver_profiling_ppi_setup(uint8_t ppi_channel1, uint8_t ppi_channel2, uint8_t ppi_channel3, uint8_t ppi_channel_4, uint8_t gpiote_output_channel);
	
void receiver_profiling_gpiote_setup(uint8_t gpiote_output_channel, uint32_t gpio_pin_output);
