# BOARD=arduino:avr:leonardo
# PORT=/dev/serial/by-id/usb-Arduino_LLC_Arduino_Leonardo-if00
# PORT=/dev/ttyACM0
SRC1=bh1750
SRC2=ddcutil-bh1750.c
SRC3=ddcutil-bh1750.service
BIN1=ddcutil-bh1750
BIN2=ddcutil-bh1750-debug

build:
	arduino-cli compile --fqbn ${BOARD} $(SRC1)
# 	musl-gcc -Os -static -idirafter -I/usr/include -L/usr/lib -o $(BIN1) $(SRC2) -lddcutil
	gcc $(SRC2) -o $(BIN1) -lddcutil
	gcc $(SRC2) -o $(BIN2) -lddcutil -DDEBUG

upload:
	arduino-cli upload -p ${PORT} --fqbn ${BOARD} $(SRC1)

install:
	cp $(BIN1) /usr/bin/$(BIN1)

init:
	cp $(SRC3) $(HOME)/.config/systemd/user/$(SRC3)

monitor:
	arduino-cli monitor -p ${PORT}

uninstall:
	rm /usr/bin/$(BIN1)

clean:
	rm ddcutil-bh1750
	rm ddcutil-bh1750-debug
