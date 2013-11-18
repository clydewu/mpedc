arm-linux-gcc -o0 -o show_screen show_screen.c -Wall -Llib -lpthread -lfreetype -lmpedc -lfreetype -L/usr/local/lib/
arm-linux-strip show_screen
chmod 755 show_screen
