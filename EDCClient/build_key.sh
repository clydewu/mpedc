arm-linux-gcc -o0 -o key_loop key_loop.c -Wall -Llib -lpthread -lfreetype -lmpedc -lfreetype -L/usr/local/lib/
arm-linux-strip key_loop
chmod 775 key_loop
