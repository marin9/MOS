CC=arm-none-eabi
CFLAGS=-mcpu=arm1176jzf-s -ffreestanding -nostdlib -O2 -Wall -Wextra -c
LDFLAGS=-nostdlib -lgcc

BOOT=boot
ARCH=arch
KERNEL=kernel
INCLUDE=include
OBJ=obj

LINKER=$(ARCH)/linker.ld
ELF=$(OBJ)/kernel.elf
TARGET=kernel.img


SRC_ASM		:= $(wildcard $(ARCH)/*.s)
SRC_ARCH	:= $(wildcard $(ARCH)/*.c)
SRC_KERNEL	:= $(wildcard $(KERNEL)/*.c)
OBJ_ASM		:= $(SRC_ASM:$(ARCH)/%.s=$(OBJ)/%.o)
OBJ_ARCH	:= $(SRC_ARCH:$(ARCH)/%.c=$(OBJ)/%.o)
OBJ_KERNEL	:= $(SRC_KERNEL:$(KERNEL)/%.c=$(OBJ)/%.o)
INCLUDES 	:= $(wildcard $(INCLUDE)/*.h)


all: $(TARGET)

clean:
	@-rm -rf $(OBJ)
	@-rm -f $(ELF)
	@-rm -f $(TARGET)
	@echo " Clean"

$(TARGET): $(OBJ) $(ELF)
	@echo " Create $(TARGET)"
	@$(CC)-objcopy $(ELF) -O binary $(TARGET)

$(ELF): $(LINKER) $(OBJ_ASM) $(OBJ_ARCH) $(OBJ_KERNEL)
	@echo " Create $(ELF)"
	@$(CC)-gcc -T $(LINKER) $(OBJ_ASM) $(OBJ_ARCH) $(OBJ_KERNEL) -o $@ $(LDFLAGS)

$(OBJ_ASM): $(OBJ)/%.o : $(ARCH)/%.s
	@echo " CC $<"
	@$(CC)-gcc $(CFLAGS) $< -o $@

$(OBJ_ARCH): $(OBJ)/%.o : $(ARCH)/%.c $(INCLUDES)
	@echo " CC $<"
	@$(CC)-gcc $(CFLAGS) $< -o $@ -I$(INCLUDE)

$(OBJ_KERNEL): $(OBJ)/%.o : $(KERNEL)/%.c $(INCLUDES)
	@echo " CC $<"
	@$(CC)-gcc $(CFLAGS) $< -o $@ -I$(INCLUDE)

$(OBJ):
	@mkdir $@
