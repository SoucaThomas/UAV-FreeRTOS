.PHONY: all firmware tests clean flash

all: firmware

firmware:
	$(MAKE) -C firmware

tests:
	$(MAKE) -C tests

clean:
	$(MAKE) -C firmware clean
	$(MAKE) -C tests clean

flash: firmware
	$(MAKE) -C firmware flash
