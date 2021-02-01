# single_chan_pkt_fwd
# Single Channel LoRaWAN Gateway

CC = gcc
QUIET ?= @
TARGET = lora_reg
CFLAGS = -g -O0 -Wall
LIBS = -lwiringPi -lpthread
INCS = -I. -I../sx1278/

source_files = lora_reg.c ../sx1278/lib_lora.c

# Create object and dependency file
objects = $(source_files:.c=.o)
depends = $(objects:.o=.d)

all : $(objects)
	@echo "Creating $(TARGET)"
	$(QUIET)$(CC) $(objects) -o $(TARGET) $(LIBS)

clean :
	@echo "Cleaning..."
	$(QUIET)rm -f $(objects)
	$(QUIET)rm -f $(depends)
	$(QUIET)rm -f $(TARGET)

ifeq ($(findstring $(MAKECMDGOALS), clean),)
-include $(depends)
endif

debug:
	@echo " Debug your makefile"
	@echo "    source_files: $(source_files)"
	@echo "    objects: $(objects)"
	@echo "    depends: $(depends)"

run: all
	./$(TARGET)

.PHONY: all clean debug run

#
# Implicit Rules
#
%.o : %.c
	@echo "Compiling $< to $@"
	$(QUIET)$(CC) $(CFLAGS) $(INCS) -c $< -o $@

%.d : %.c
	@echo "Creating $@"
	$(QUIET)$(CC) $(INCS) -MM $< > $@
