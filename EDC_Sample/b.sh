arm-linux-gcc -O0 -o EDC_Sample EDC_Sample.c -Wall -Llib -lpthread -lfreetype -lmpedc -lfreetype -L/usr/local/lib/
arm-linux-strip EDC_Sample
chmod 755 EDC_Sample
