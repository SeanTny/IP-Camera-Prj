all:
	arm-linux-gcc -c -o  show_font.o  show_font.c
	arm-linux-gcc -c -o  font_data.o  font_data.c
	arm-linux-gcc -o test show_font.o font_data.o	
	rm -rf *.o	
clean:
	rm -rf *.o



#all:
#	arm-linux-gcc -finput-charset=UTF-8  -fexec-charset=GBK -o show_font2 show_font.c
#
#clean:
#	rm -rf *.o

