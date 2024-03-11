#pragma once

#include <driver/gpio.h>
#include <esp_err.h>
#include <driver/sdspi_host.h>
#include <sdmmc_cmd.h>

class SD_Card {
    struct Pins {
        static const gpio_num_t miso { GPIO_NUM_15 };
        static const gpio_num_t mosi { GPIO_NUM_23 };
        static const gpio_num_t clk  { GPIO_NUM_22 };
        static const gpio_num_t cs   { GPIO_NUM_21 };
    };

    // This is just = SDSPI_HOST_DEFAULT() but max_freq_khz modified
    static constexpr sdmmc_host_t host {
        .flags = SDMMC_HOST_FLAG_SPI | SDMMC_HOST_FLAG_DEINIT_ARG,
        .slot = SDSPI_DEFAULT_HOST,
        .max_freq_khz = SDMMC_FREQ_DEFAULT,
        .io_voltage = 3.3f,
        .init = &sdspi_host_init,
        .set_bus_width = NULL,
        .get_bus_width = NULL,
        .set_bus_ddr_mode = NULL,
        .set_card_clk = &sdspi_host_set_card_clk,
        .set_cclk_always_on = NULL,
        .do_transaction = &sdspi_host_do_transaction,
        .deinit_p = &sdspi_host_remove_device,
        .io_int_enable = &sdspi_host_io_int_enable,
        .io_int_wait = &sdspi_host_io_int_wait,
        .command_timeout_ms = 0,
        .get_real_freq = &sdspi_host_get_real_freq,
        .input_delay_phase = SDMMC_DELAY_PHASE_0,
        .set_input_delay = NULL
    };

    static constexpr spi_bus_config_t bus_cfg {
        .mosi_io_num = Pins::mosi,
        .miso_io_num = Pins::miso,
        .sclk_io_num = Pins::clk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE,
        .flags = SPICOMMON_BUSFLAG_MASTER,
        .isr_cpu_id = ESP_INTR_CPU_AFFINITY_0,
        .intr_flags = 0,
    };

    // This corresponds to SDSPI_DEVICE_CONFIG_DEFAULT() just .host_id and .gpio_cs is modified
    static constexpr sdspi_device_config_t slot_config {
        .host_id   = spi_host_device_t(host.slot),
        .gpio_cs   = Pins::cs,
        .gpio_cd   = SDSPI_SLOT_NO_CD,
        .gpio_wp   = SDSPI_SLOT_NO_WP,
        .gpio_int  = SDSPI_SLOT_NO_INT,
        .gpio_wp_polarity = SDSPI_IO_ACTIVE_LOW,
    };

    sdspi_dev_handle_t handle;
private:
    SD_Card();
    esp_err_t init();
public:
    sdmmc_card_t card {};
    static SD_Card& get_instance();
    esp_err_t deinit();
    esp_err_t check_all_blocks();
};