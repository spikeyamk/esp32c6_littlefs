#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <array>
#include <string>

#include <littlefs/lfs.h>
#include <trielo/trielo.hpp>

#include "lfs_port/sdmmc/test.hpp"

namespace LFS {
    namespace SDMMC {
        int read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
            assert(off == 0);
            assert(size == c->block_size);
            assert(block < c->block_count);
            sdmmc_card_t* card = static_cast<sdmmc_card_t*>(c->context);
            assert(sdmmc_read_sectors(card, buffer, block, 1) == ESP_OK);
            return LFS_ERR_OK;
        }

        int erase(const struct lfs_config *c, lfs_block_t block) {
            assert(block < c->block_count);
            sdmmc_card_t* card = static_cast<sdmmc_card_t*>(c->context);
            assert(sdmmc_erase_sectors(card, block, 1, SDMMC_ERASE_ARG) == ESP_OK);
            return LFS_ERR_OK;
        }

        int prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) {
            assert(off == 0);
            assert(size == c->block_size);
            assert(block < c->block_count);
            sdmmc_card_t* card = static_cast<sdmmc_card_t*>(c->context);
            assert(sdmmc_write_sectors(card, buffer, block, 1) == ESP_OK);
            return LFS_ERR_OK;
        }

        int sync(const struct lfs_config *c) {
            (void) c;
            return LFS_ERR_OK;
        }

        int test(sdmmc_card_t* card) {
            const lfs_size_t block_size { 512 };
            const lfs_size_t block_count { static_cast<lfs_size_t>(card->csd.capacity) };
            static uint8_t read_buffer[block_size];
            static uint8_t prog_buffer[block_size];
            static uint8_t lookahead_buffer[block_size];

            const lfs_config config {
                .context = static_cast<void*>(card),
                .read = read,
                .prog = prog,
                .erase = erase,
                .sync = sync,
            #ifdef LFS_THREADSAFE
                .lock = nullptr,
                .unlock = nullptr,
            #endif
                .read_size = block_size,
                .prog_size = block_size,
                .block_size = block_size,
                .block_count = block_count,
                .block_cycles = -1,
                .cache_size = block_size,
                .lookahead_size = block_size,
                .compact_thresh = static_cast<lfs_size_t>(-1),

                .read_buffer = static_cast<void*>(read_buffer),
                .prog_buffer = static_cast<void*>(prog_buffer),
                .lookahead_buffer = static_cast<void*>(lookahead_buffer),

                .name_max = 0,
                .file_max = 0,
                .attr_max = 0,
                .metadata_max = 0,
                .inline_max = static_cast<lfs_size_t>(-1),
            };

            lfs_t lfs;

            if(Trielo::trielo<lfs_format>(Trielo::OkErrCode<int>(LFS_ERR_OK), &lfs, &config) != LFS_ERR_OK) {
                return 1;
            }

            // mount the filesystem
            if(Trielo::trielo<lfs_mount>(Trielo::OkErrCode<int>(LFS_ERR_OK), &lfs, &config) != LFS_ERR_OK) {
                return 2;
            }

            static const std::array<uint8_t, 512> message { 
                0x52, 0x20, 0xeb, 0x24, 0xe8, 0x1f, 0xa5, 0x66, 
                0x00, 0x16, 0xc7, 0x65, 0x42, 0x5c, 0x3c, 0x71, 
                0x52, 0x6c, 0x2c, 0x0f, 0x36, 0xc2, 0x92, 0x52, 
                0xc1, 0x4f, 0x86, 0x0c, 0x72, 0x69, 0x06, 0x95, 
                0x54, 0x92, 0x55, 0x3e, 0xf4, 0xaf, 0x12, 0x2e, 
                0x39, 0xc0, 0xf6, 0x9a, 0x9b, 0x19, 0x91, 0xc9, 
                0xb3, 0xbd, 0xd0, 0x85, 0xbc, 0x1a, 0xed, 0xb5, 
                0xce, 0x09, 0x1a, 0x68, 0x75, 0xd9, 0x42, 0xdc, 
                0x18, 0xf7, 0xed, 0x27, 0xcb, 0x8a, 0x22, 0xb6, 
                0x8b, 0xf9, 0x0c, 0x7b, 0x01, 0x0a, 0x57, 0x66, 
                0x75, 0x45, 0x72, 0x48, 0xf6, 0xdd, 0x72, 0x16, 
                0x9e, 0x99, 0x87, 0x1f, 0x12, 0x1e, 0x61, 0x20, 
                0x66, 0xf9, 0x9b, 0x08, 0x1c, 0x3e, 0x8d, 0xa5, 
                0xfd, 0x2a, 0x4c, 0xbe, 0xd7, 0x44, 0xfc, 0x30, 
                0x87, 0x7a, 0x15, 0xd4, 0x18, 0x53, 0x35, 0xb3, 
                0xad, 0x59, 0x2d, 0xb0, 0xf1, 0xaf, 0x4a, 0xff, 
                0x06, 0x3b, 0x95, 0x75, 0xef, 0xb2, 0x1b, 0x2f, 
                0x56, 0x31, 0x09, 0x93, 0x8d, 0x1a, 0x50, 0x68, 
                0xcd, 0x81, 0x16, 0x47, 0x6d, 0xa0, 0x5f, 0x04, 
                0xc9, 0x49, 0x5c, 0x60, 0xc9, 0x7a, 0xf4, 0x3d, 
                0x9c, 0x73, 0xea, 0x66, 0x87, 0x4d, 0xfb, 0xb6, 
                0x7a, 0xc3, 0x56, 0x6f, 0xb9, 0x58, 0x49, 0x8c, 
                0x6b, 0x92, 0x14, 0xbc, 0x9f, 0x78, 0x5c, 0x63, 
                0x97, 0xb1, 0x41, 0xd2, 0x19, 0x71, 0x11, 0xc0, 
                0x15, 0xa2, 0x1f, 0x4b, 0x76, 0x80, 0x2b, 0xf2, 
                0xf6, 0xe1, 0x28, 0xcd, 0x0d, 0x3f, 0xca, 0xe8, 
                0x9c, 0x90, 0x51, 0x34, 0xc5, 0xdc, 0x0f, 0x6d, 
                0xc6, 0xb0, 0x88, 0x47, 0x13, 0x4b, 0xac, 0x8d, 
                0x52, 0x95, 0xf1, 0xec, 0xed, 0xe0, 0x99, 0x9b, 
                0x3d, 0xde, 0x14, 0xd3, 0x8a, 0xb3, 0xd9, 0x4f, 
                0x23, 0x2e, 0xb2, 0x66, 0xec, 0x98, 0xa8, 0x09, 
                0x0d, 0x7d, 0xc8, 0xf3, 0x61, 0x3d, 0x39, 0x6a, 
                0x00, 0xc2, 0x0e, 0x65, 0x3d, 0xda, 0x3d, 0xce, 
                0x82, 0xc8, 0x12, 0x37, 0x30, 0xfb, 0xd6, 0x3b, 
                0x47, 0xbb, 0xf9, 0x68, 0xce, 0x2d, 0x27, 0x0e, 
                0xc7, 0xd3, 0x8f, 0x47, 0x89, 0xeb, 0x21, 0xfb, 
                0x0e, 0xd4, 0x47, 0xd3, 0xcf, 0x54, 0xa6, 0xac, 
                0x38, 0xed, 0xf2, 0xa5, 0xf6, 0x41, 0xe0, 0xae, 
                0xa1, 0xa9, 0xfe, 0xf1, 0x23, 0x9c, 0x47, 0xf4, 
                0x24, 0xa6, 0x74, 0x57, 0xd5, 0x8b, 0xe9, 0xa4, 
                0x9a, 0x75, 0xda, 0x28, 0xdd, 0xf1, 0x2e, 0x84, 
                0x14, 0x07, 0x2f, 0x47, 0xb8, 0xb8, 0xbb, 0x41, 
                0xf4, 0x1a, 0x2b, 0x7d, 0xb2, 0x32, 0x23, 0x6e, 
                0xc9, 0x80, 0x9e, 0x10, 0x07, 0x3d, 0x6d, 0xaa, 
                0x5b, 0x08, 0x61, 0x79, 0xfd, 0xd4, 0xdf, 0x7a, 
                0x3b, 0xb7, 0x54, 0x9b, 0x84, 0xb0, 0x2c, 0xff, 
                0xce, 0x23, 0x84, 0x53, 0x59, 0x33, 0xff, 0xa4, 
                0xe6, 0x7b, 0x54, 0xd4, 0x81, 0x5e, 0xad, 0xb9, 
                0x9a, 0x60, 0x64, 0x3e, 0x93, 0x5e, 0xec, 0x2e, 
                0xbb, 0xac, 0x02, 0x57, 0xd6, 0x33, 0xdc, 0xea, 
                0xbe, 0xe4, 0xb1, 0x56, 0x19, 0x7d, 0xa0, 0x16, 
                0x0c, 0xed, 0x2a, 0x2b, 0xd4, 0x38, 0x1d, 0x75, 
                0x5d, 0x35, 0x07, 0xf9, 0x19, 0xc3, 0x5e, 0x1c, 
                0xa5, 0x94, 0x08, 0xa1, 0x8e, 0x0e, 0x73, 0xda, 
                0x39, 0x32, 0x92, 0xb9, 0x62, 0x62, 0xa8, 0x68, 
                0xb0, 0xa5, 0xf8, 0x18, 0x1d, 0x2e, 0x63, 0x4c, 
                0x1b, 0xea, 0xe9, 0xc0, 0x80, 0xf6, 0xec, 0x7d, 
                0x50, 0x96, 0x3f, 0x5c, 0x82, 0x1d, 0xe8, 0x44, 
                0x5f, 0x01, 0x3b, 0x20, 0xea, 0x58, 0xc6, 0x56, 
                0x43, 0x93, 0x2c, 0xe9, 0x7d, 0x7e, 0xf5, 0x6f, 
                0x8d, 0x06, 0x36, 0x75, 0xc6, 0x2b, 0x1e, 0xd0, 
                0x74, 0xf9, 0x51, 0x86, 0x8a, 0x75, 0xdc, 0x3c, 
                0x76, 0xe8, 0xfe, 0xd7, 0x52, 0xd6, 0x83, 0x71, 
                0xb4, 0x21, 0x32, 0x77, 0x5e, 0xc0, 0x5b, 0x11, 
            };
            static std::array<uint8_t, message.size()> read_message;

            for(size_t i = 32; (i * (512)) <= (32 * (2 * 512)); i++) {
                const std::string name { std::string("test").append(std::to_string(i)) };

                lfs_file_t write_file;
                Trielo::trielo<lfs_file_open>(Trielo::OkErrCode<int>(LFS_ERR_OK), &lfs, &write_file, name.c_str(), LFS_O_RDWR | LFS_O_CREAT);
                for(size_t y = 0; y < i; y++) {
                    const lfs_ssize_t ret_write { lfs_file_write(&lfs, &write_file, static_cast<const void*>(message.data()), message.size()) };
                    if(ret_write != static_cast<lfs_ssize_t>(message.size())) {
                        std::printf("lfs_file_write(&lfs, &write_file, static_cast<const void*>(message.data()), message.size()) != static_cast<lfs_ssize_t>(message.size())\n");
                        std::printf("ret_write: %ld\n", ret_write);
                        return -1 * static_cast<int>(i);
                    }
                }
                Trielo::trielo<lfs_file_close>(Trielo::OkErrCode<int>(LFS_ERR_OK), &lfs, &write_file);

                lfs_file_t read_file;
                Trielo::trielo<lfs_file_open>(Trielo::OkErrCode<int>(LFS_ERR_OK), &lfs, &read_file, name.c_str(), LFS_O_RDONLY);
                read_message.fill(0);
                for(size_t y = 0; y < i; y++) {
                    const lfs_ssize_t ret_read { lfs_file_read(&lfs, &read_file, static_cast<void*>(read_message.data()), read_message.size()) };
                    if(read_message != message) {
                        std::printf("read_message != message\n");
                        std::printf("ret_read: %ld\n", ret_read);
                        return -1 * static_cast<int>(i);
                    }
                }
                Trielo::trielo<lfs_file_close>(Trielo::OkErrCode<int>(LFS_ERR_OK), &lfs, &read_file);
            }

            /*
            for(size_t i = 0; i < 200; i++) {
                const std::string name { std::string("test").append(std::to_string(i)) };

                lfs_file_t write_file;
                Trielo::trielo<lfs_file_open>(Trielo::OkErrCode<int>(LFS_ERR_OK), &lfs, &write_file, name.c_str(), LFS_O_RDWR | LFS_O_CREAT);
                const lfs_ssize_t ret_write { lfs_file_write(&lfs, &write_file, static_cast<const void*>(message.data()), message.size()) };
                if(ret_write != static_cast<lfs_ssize_t>(message.size())) {
                    std::printf("lfs_file_write(&lfs, &write_file, static_cast<const void*>(message.data()), message.size()) != static_cast<lfs_ssize_t>(message.size())\n");
                    std::printf("ret_write: %ld\n", ret_write);
                    return -1 * static_cast<int>(i);
                }
                Trielo::trielo<lfs_file_close>(Trielo::OkErrCode<int>(LFS_ERR_OK), &lfs, &write_file);

                lfs_file_t read_file;
                Trielo::trielo<lfs_file_open>(Trielo::OkErrCode<int>(LFS_ERR_OK), &lfs, &read_file, name.c_str(), LFS_O_RDONLY);

                read_message.fill(0);
                const lfs_ssize_t ret_read { lfs_file_read(&lfs, &read_file, static_cast<void*>(read_message.data()), read_message.size()) };
                if(read_message != message) {
                    std::printf("read_message != message\n");
                    std::printf("ret_read: %ld\n", ret_read);
                    return -1 * static_cast<int>(i);
                }
                Trielo::trielo<lfs_file_close>(Trielo::OkErrCode<int>(LFS_ERR_OK), &lfs, &read_file);
            }
            */

            if(Trielo::trielo<lfs_unmount>(Trielo::OkErrCode<int>(LFS_ERR_OK), &lfs) != LFS_ERR_OK) {
                return 3;
            }

            return 0;
        }
    }
}
