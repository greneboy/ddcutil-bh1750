BOARD=arduino:avr:nano
PORT=/dev/serial/by-id/usb-YourDeviceID
SRC1=bh1750
SRC2=ddcutil-bh1750.c
BIN1=ddcutil-bh1750

build:
	#arduino-cli compile --fqbn $(BOARD) $(SRC1)
	musl-gcc -Os -static -I/usr/include -I/usr/include/ddcutil -L/usr/lib -lddcutil -o $(BIN1) $(SRC2)

upload:
	arduino-cli upload -p $(PORT) --fqbn $(BOARD) $(SRC1)

install:
	cp $(BIN1) /usr/bin/$(BIN1)

monitor:
	screen $(PORT) 9600
