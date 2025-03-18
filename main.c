/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/* STEP 1.2 - Include the header files for SPI, GPIO and devicetree */
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

LOG_MODULE_REGISTER(Lesson5_Exercise1, LOG_LEVEL_INF);

const struct gpio_dt_spec ledspec = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);

/* STEP 3 - Retrieve the API-device structure */
#define SPIOP	SPI_WORD_SET(8) | SPI_TRANSFER_MSB
struct spi_dt_spec spispec = SPI_DT_SPEC_GET(DT_NODELABEL(bme280), SPIOP, 0);

uint16_t read_ambient_light(void) {
    uint8_t rx_buffer[2] = {0};
    struct spi_buf rx_buf = { .buf = rx_buffer, .len = sizeof(rx_buffer) };
    struct spi_buf_set rx = { .buffers = &rx_buf, .count = 1 };

    spi_read_dt(&spispec, &rx);
    
    // The ADC081S021 returns a 12-bit value (left-aligned in 16-bit data)
    return ((rx_buffer[0] << 8) | rx_buffer[1]) >> 4;
}

int main(void)
{
	int err;
	gpio_pin_configure_dt(&ledspec, GPIO_OUTPUT_ACTIVE);
	
	err = gpio_is_ready_dt(&ledspec);
	if (!err) {
		LOG_ERR("Error: GPIO device is not ready, err: %d", err);
		return 0;
	}
	/* STEP 10.1 - Check if SPI and GPIO devices are ready */
	err = spi_is_ready_dt(&spispec);
	if (!err) {
		LOG_ERR("Error: SPI device is not ready, err: %d", err);
		return 0;
	}
	
	while (1) {
        uint16_t light_value = read_ambient_light();
        printk("Ambient Light: %d\n", light_value);
		gpio_pin_toggle_dt(&ledspec);
        k_sleep(K_MSEC(1000));
    }

	return 0;
}
