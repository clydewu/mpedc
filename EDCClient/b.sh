arm-linux-gcc -O0 -o test.out buzzer.c -Wall -Llib -lpthread -lfreetype -lmpedc -lfreetype -L/usr/local/lib/
arm-linux-strip test.out
chmod 755 test.out
