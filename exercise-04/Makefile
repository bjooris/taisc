
BSL = ibcn-f5x-tos-bsl
BSL_OPT = -5 -R --invert-reset --swap-reset-test -r -e -I

DEV ?= /dev/ttyUSB0

main: ../util/taisc.o bytecode.o appConfig.o
	msp430-gcc -o main ../util/taisc.o bytecode.o appConfig.o -mmcu=msp430f5437
clean:
	rm -f bytecode.c bytecode.o appConfig.o main main.hex

bytecode.c: chain.c
	python ../util/parser.py chain.c bytecode.c

install: main
	msp430-objcopy -O ihex main main.hex
	$(BSL) $(BSL_OPT) -c $(DEV) -p main.hex
	
bytecode.o: bytecode.c
	msp430-gcc -c -o bytecode.o bytecode.c -mmcu=msp430f5437

appConfig.o: ../appConfig.c
	msp430-gcc -c -o appConfig.o ../appConfig.c -mmcu=msp430f5437
