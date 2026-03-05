CROSS=i386-elf
AS=$(CROSS)-as
CC=$(CROSS)-gcc
LD=$(CROSS)-ld
CFLAGS=-ffreestanding -m32 -Iinclude -Iinclude/apps
LDFLAGS=-T linker.ld

KERNEL_OBJS = \
  boot/boot.o \
  kernel/entry.o \
  kernel/vga.o \
  kernel/keyboard.o \
  kernel/mouse.o \
  kernel/fs.o \
  kernel/desktop.o \
  kernel/window.o \
  kernel/shutdown.o \
  kernel/apps/files.o \
  kernel/apps/ddtexts.o \
  kernel/apps/calculator.o \
  kernel/apps/snake.o \
  kernel/apps/trash.o \
  kernel/string.o \
  kernel/font8x8_basic.o \
  kernel/io.o

INSTALLER_OBJS = \
  installer/boot.o \
  installer/entry.o \
  installer/copy.o \
  installer/stylus.o \
  installer/progress.o \
  installer/partition.o \
  installer/syscalls.o \
  installer/stubs.o \
  installer/kernel_bin.o \
  installer/grub_cfg.o \
  installer/ddsuite_pkg.o \
  kernel/vga.o \
  kernel/fs.o \
  kernel/font8x8_basic.o \
  kernel/io.o \
  kernel/string.o

all: dharos.iso

boot/boot.o: boot/boot.S
	$(AS) boot/boot.S -o $@

installer/boot.o: installer/boot.S
	$(AS) installer/boot.S -o $@

kernel/io.o: kernel/io.c
	$(CC) $(CFLAGS) -c kernel/io.c -o kernel/io.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: $(KERNEL_OBJS)
	$(LD) -nostdlib $(LDFLAGS) -o kernel.bin $(KERNEL_OBJS)

installer.bin: $(INSTALLER_OBJS)
	$(LD) -nostdlib $(LDFLAGS) -o installer.bin $(INSTALLER_OBJS)

installer/kernel_bin.o: kernel.bin
	objcopy -I binary -O elf32-i386 -B i386 kernel.bin installer/kernel_bin.o

installer/grub_cfg.o: assets/grub.cfg
	objcopy -I binary -O elf32-i386 -B i386 assets/grub.cfg installer/grub_cfg.o

installer/ddsuite_pkg.o: assets/ddsuite.ddpkg
	objcopy -I binary -O elf32-i386 -B i386 assets/ddsuite.ddpkg installer/ddsuite_pkg.o

dharos.iso: kernel.bin installer.bin assets/grub.cfg
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/kernel.bin
	cp installer.bin iso/boot/installer.bin
	cp assets/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o dharos.iso iso

clean:
	rm -rf *.bin *.o */*.o */*/*.o iso dharos.iso