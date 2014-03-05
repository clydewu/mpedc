arm-linux-gcc -O0 -g -o EDCClient EDCClient.c -Wall -Llib -lpthread -lfreetype -lmpedc -lfreetype -L/usr/local/lib/
arm-linux-strip EDCClient
chmod 755 EDCClient


