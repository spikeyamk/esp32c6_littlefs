#include <trielo/trielo.hpp>
#include <functional>

#include "lfs_port/mem/test.hpp"
#include "sd_card.hpp"
#include "lfs_port/sdmmc/test.hpp"

esp_err_t all_block_test() {
    SD_Card& sd_card { SD_Card::get_instance() };

    /*
    const esp_err_t ret { sd_card.check_all_blocks() };
    if(ret != ESP_OK) {
        return ret;
    }
    */

    sd_card.deinit();
    return ESP_OK;
}

void littlefs_test() {
    SD_Card& sd_card { SD_Card::get_instance() };
    Trielo::trielo<LFS::SDMMC::test>(Trielo::OkErrCode(ESP_OK), &sd_card.card);
    sd_card.deinit();
}

extern "C" void app_main() {
    //Trielo::trielo<all_block_test>(Trielo::OkErrCode(ESP_OK));
    littlefs_test();
}