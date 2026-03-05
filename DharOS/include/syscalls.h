#ifndef SYSCALLS_H
#define SYSCALLS_H

// 💾 Installer syscalls for writing files and rebooting
void syscall_write_file(int partition, const char* path, void* data, unsigned int size);
void reboot();

#endif