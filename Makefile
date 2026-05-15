.PHONY: all uav ground tests clean flash flash-stlink debug

all: uav

uav:
	$(MAKE) -C firmware APP=uav

ground:
	$(MAKE) -C firmware APP=ground

tests:
	$(MAKE) -C tests

clean:
	$(MAKE) -C firmware clean
	$(MAKE) -C tests clean

flash: uav
	$(MAKE) -C firmware APP=uav flash

flash-ground: ground
	$(MAKE) -C firmware APP=ground flash

flash-stlink: uav
	$(MAKE) -C firmware APP=uav flash-stlink

flash-stlink-ground: ground
	$(MAKE) -C firmware APP=ground flash-stlink

debug: uav
	./debug.sh firmware/build/uav/uav.elf

debug-ground: ground
	./debug.sh firmware/build/ground/ground.elf
