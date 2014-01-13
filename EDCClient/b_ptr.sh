arm-linux-gcc -o ptr_init ptr_init.c -Wall -Llib -lpthread -lfreetype -lmpedc -lfreetype -L/usr/local/lib/
arm-linux-strip ptr_init
chmod 775 ptr_init
