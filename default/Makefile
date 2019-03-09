###############################################################################
# Makefile for the project Würgertime
###############################################################################

## General Flags
PROJECT = Wuergertime
GAME= Wuergertime
MCU = atmega644
TARGET = $(GAME).elf
CC = avr-gcc
INFO=../gameinfo.properties
AVR_SYSROOT=/usr/avr/sys-root/

## Escape spaces in mixer path (due to custom sounds.inc)
#EMPTY:=
#SPACE:= $(EMPTY) $(EMPTY)
#SPACE_ESC:= \\$(SPACE)
#MIX_PATH:= $(realpath ../data/sounds.inc)
#MIX_PATH_ESC:= $(subst $(SPACE),$(SPACE_ESC),$(MIX_PATH))

## Kernel settings
KERNEL_DIR = ../../../kernel
KERNEL_OPTIONS  = -DVIDEO_MODE=3 -DINTRO_LOGO=0
KERNEL_OPTIONS += -DSOUND_MIXER=1
KERNEL_OPTIONS += -DSCROLLING=0
KERNEL_OPTIONS += -DMAX_SPRITES=24
KERNEL_OPTIONS += -DRAM_TILES_COUNT=32

#KERNEL_OPTIONS += -DSOUND_CHANNEL_4_ENABLE=0 -DSOUND_CHANNEL_2_ENABLE=1 -DSOUND_CHANNEL_3_ENABLE=1 -DMIXER_CHAN4_TYPE=1
#KERNEL_OPTIONS  += -DMIXER_WAVES=\"$(MIX_PATH_ESC)\"

## Options common to compile, link and assembly rules
COMMON = --sysroot=$(AVR_SYSROOT) -mmcu=$(MCU)
#-DDEBUG
#--verbose

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -Os -fsigned-char -ffunction-sections
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d
CFLAGS += $(KERNEL_OPTIONS)

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=$(GAME).map
LDFLAGS += -Wl,-gc-sections
#LDFLAGS += -Wl,--section-start,.noinit=0x800100 -Wl,--section-start,.data=0x800500  SCROLLING ONLY!

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = uzeboxVideoEngineCore.o uzeboxCore.o uzeboxSoundEngine.o uzeboxSoundEngineCore.o uzeboxVideoEngine.o utils.o controllers.o tiles.o draw.o sprites.o opponents.o pepper.o player.o highscores.o screens.o screens_ingame.o screens_outofgame.o $(GAME).o

## Objects explicitly added by the user
LINKONLYOBJECTS =

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)" -I"$(AVR_SYSROOT)/include"



## Build.
all: $(TARGET) ../data/tiles0.inc ../data/sprites0.inc $(GAME).hex $(GAME).eep $(GAME).lss $(GAME).uze size


## Compile Kernel files.
uzeboxVideoEngineCore.o: $(KERNEL_DIR)/uzeboxVideoEngineCore.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

uzeboxSoundEngineCore.o: $(KERNEL_DIR)/uzeboxSoundEngineCore.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

uzeboxCore.o: $(KERNEL_DIR)/uzeboxCore.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzeboxSoundEngine.o: $(KERNEL_DIR)/uzeboxSoundEngine.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzeboxVideoEngine.o: $(KERNEL_DIR)/uzeboxVideoEngine.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<


## Convert tilesets.
../data/tiles0.inc: ../data/tiles.pcx
	../tools/pcxtotiles.tcl Tiles <$^ >$@
../data/sprites0.inc: ../data/sprites.pcx
	../tools/pcxtotiles.tcl SpriteTiles <$^ >$@


## Compile game sources
$(GAME).o: ../$(GAME).c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

utils.o: ../utils.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

controllers.o: ../controllers.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

sprites.o: ../sprites.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

opponents.o: ../opponents.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

pepper.o: ../pepper.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

player.o: ../player.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

tiles.o: ../tiles.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

draw.o: ../draw.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

highscores.o: ../highscores.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

screens.o: ../screens.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

screens_outofgame.o: ../screens_outofgame.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<

screens_ingame.o: ../screens_ingame.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<


## Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(LIBDIRS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

%.uze: $(TARGET)
	@echo
	-packrom $(GAME).hex $@ $(INFO)

UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')
AVRSIZEFLAGS := -A ${TARGET}
ifneq (,$(findstring MINGW,$(UNAME)))
AVRSIZEFLAGS := -C --mcu=${MCU} ${TARGET}
endif

size: ${TARGET}
	@echo
	@avr-size ${AVRSIZEFLAGS}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(GAME).* dep/*

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

