#include "fs.h"
#include "ata.h"
#include "vga.h"
#include <string.h>
#include <stddef.h>

File    files[MAX_FILES];
uint8_t file_used[MAX_FILES];

static void fs_mem_rebuild_used() {
    for (int i = 0; i < MAX_FILES; i++) file_used[i] = files[i].used;
}

// Flatten files[] onto disk sectors and restore

void save_fs() {
    // Write files[] across FS_TABLE_SECTORS sectors starting FS_START_LBA
    uint8_t sector[FS_SECTOR_SIZE];
    const uint8_t* src = (const uint8_t*)files;

    uint32_t total = MAX_FILES * sizeof(File);
    for (uint32_t s = 0; s < FS_TABLE_SECTORS; s++) {
        uint32_t offset = s * FS_SECTOR_SIZE;
        uint32_t remain = (total > offset) ? (total - offset) : 0;
        uint32_t chunk = remain > FS_SECTOR_SIZE ? FS_SECTOR_SIZE : remain;
        // Copy chunk; pad rest with zeros
        if (chunk > 0) memcpy(sector, src + offset, chunk);
        if (chunk < FS_SECTOR_SIZE) memset(sector + chunk, 0, FS_SECTOR_SIZE - chunk);

        ata_write_sector(FS_START_LBA + s, sector);
    }
}

void load_fs() {
    uint8_t sector[FS_SECTOR_SIZE];
    uint8_t* dst = (uint8_t*)files;

    uint32_t total = MAX_FILES * sizeof(File);
    for (uint32_t s = 0; s < FS_TABLE_SECTORS; s++) {
        uint32_t offset = s * FS_SECTOR_SIZE;
        uint32_t remain = (total > offset) ? (total - offset) : 0;
        uint32_t chunk = remain > FS_SECTOR_SIZE ? FS_SECTOR_SIZE : remain;

        if (ata_read_sector(FS_START_LBA + s, sector) != 0) {
            // If read fails, leave memory as zeroed
            continue;
        }
        if (chunk > 0) memcpy(dst + offset, sector, chunk);
    }

    fs_mem_rebuild_used();
}

// Core operations

void write_file(const char* path, const char* data, uint32_t size) {
    // If file exists, overwrite it
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strncmp(files[i].name, path, MAX_FILENAME) == 0) {
            uint32_t n = size > MAX_FILE_SIZE ? MAX_FILE_SIZE : size;
            memcpy(files[i].data, data, n);
            files[i].size = n;
            draw_string(20, 180, "File updated.", 0x0F);
            save_fs();
            return;
        }
    }
    // Else find free slot
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            memset(files[i].name, 0, MAX_FILENAME);
            strncpy(files[i].name, path, MAX_FILENAME - 1);
            uint32_t n = size > MAX_FILE_SIZE ? MAX_FILE_SIZE : size;
            memcpy(files[i].data, data, n);
            files[i].size = n;
            files[i].used = 1;
            file_used[i] = 1;
            draw_string(20, 180, "File written.", 0x0F);
            save_fs();
            return;
        }
    }
    draw_string(20, 180, "No space left in file system.", 0x0F);
}

char* read_file(const char* path, uint32_t* out_size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strncmp(files[i].name, path, MAX_FILENAME) == 0) {
            if (out_size) *out_size = files[i].size;
            return files[i].data;
        }
    }
    if (out_size) *out_size = 0;
    draw_string(20, 180, "File not found.", 0x0F);
    return NULL;
}

void rename_file(const char* oldname, const char* newname) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strncmp(files[i].name, oldname, MAX_FILENAME) == 0) {
            memset(files[i].name, 0, MAX_FILENAME);
            strncpy(files[i].name, newname, MAX_FILENAME - 1);
            draw_string(20, 180, "File renamed.", 0x0F);
            save_fs();
            return;
        }
    }
    draw_string(20, 180, "File not found.", 0x0F);
}

void delete_file(const char* path) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strncmp(files[i].name, path, MAX_FILENAME) == 0) {
            files[i].used = 0;
            files[i].size = 0;
            memset(files[i].name, 0, MAX_FILENAME);
            memset(files[i].data, 0, MAX_FILE_SIZE);
            file_used[i] = 0;
            draw_string(20, 180, "File deleted.", 0x0F);
            save_fs();
            return;
        }
    }
    draw_string(20, 180, "File not found.", 0x0F);
}