all:
	pebble build

clean:
	pebble clean

install:
	pebble install --emulator basalt

kill:
	pebble kill

log:
	pebble logs
