
TARGET = etherrape

# microcontroller and project specific settings (everything can be overridden using config.mk)
#F_CPU = 16000000UL
#MCU = atmega32

# export current directory to use in sub-makefiles
export CURDIR

# include avr-generic makefile configuration
include $(CURDIR)/avr.mk

SRC = $(shell echo *.c)
OBJECTS += $(patsubst %.c,%.o,${SRC}) uip/uip.o uip/uip_arp.o
#CFLAGS += -Werror
#CFLAGS += -Iuip/ -Iuip/apps
#LDFLAGS += -L/usr/local/avr/avr/lib

# no safe mode checks, since the bootloader doesn't support this
AVRDUDE_FLAGS += -u

# Name of Makefile for make depend
MAKEFILE = Makefile

.PHONY: all ethcmd

all: $(TARGET).hex $(TARGET).eep.hex $(TARGET).lss ethcmd
	@echo "==============================="
	@echo "$(TARGET) compiled for: $(MCU)"
	@echo -n "size is: "
	@$(SIZE) -A $(TARGET).hex | grep "\.sec1" | tr -s " " | cut -d" " -f2
	@echo "==============================="

$(TARGET): $(OBJECTS) $(TARGET).o

$(OBJECTS): config.mk

ethcmd:
	$(MAKE) -e -C ethcmd

# subdir magic
%/% %/%.o %/%.hex %/all %/depend %/install %/clean:
	$(MAKE) -C $(@D) -e $(@F)

.PHONY: install

install: program-serial-$(TARGET)

install-eeprom: program-serial-eeprom-$(TARGET)


.PHONY: clean clean-$(TARGET) distclean

clean: clean-$(TARGET) ethcmd/clean uip/clean bootloader/clean

clean-$(TARGET):
	rm -f $(TARGET)
	rm -f $(OBJECTS)

distclean: clean
	$(MAKE) -C ethcmd distclean

depend:
	$(CC) $(CFLAGS) -M $(CDEFS) $(CINCS) $(SRC) $(ASRC) > $(MAKEFILE).dep

-include $(MAKEFILE).dep