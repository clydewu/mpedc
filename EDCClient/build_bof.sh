arm-linux-gcc -o0 -o test_bof test_bof.c -Wall -Llib -lpthread -lfreetype -lmpedc -lfreetype -L/usr/local/lib/
arm-linux-strip test_bof
chmod 775 test_bof
