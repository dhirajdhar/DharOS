#ifndef FS_H
#define FS_H
#include <stdint.h>

#define MAX_FILES     32
#define MAX_FILENAME  32
#define MAX_FILE_SIZE 4096

typedef struct {
    char name[MAX_FILENAME];
    char data[MAX_FILE_SIZE];
    uint32_t size;
    uint8_t used;
} File;

// On-disk layout: write files[] sequentially to a fixed LBA range
#define FS_START_LBA  2048  // start at 1MiB offset (safe on many emulators)
#define FS_SECTOR_SIZE 512
#define FS_TABLE_BYTES (MAX_FILES * sizeof(File))
#define FS_TABLE_SECTORS ((FS_TABLE_BYTES + FS_SECTOR_SIZE - 1) / FS_SECTOR_SIZE)

extern File files[MAX_FILES];
extern uint8_t file_used[MAX_FILES];

void save_fs();
void load_fs();

void write_file(const char* path, const char* data, uint32_t size);
char* read_file(const char* path, uint32_t* out_size);
void rename_file(const char* oldname, const char* newname);
void delete_file(const char* path);

#endif