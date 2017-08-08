all:
	arm-linux-gcc                                        -c -o font_data.o  ../03th_lcd_angle/font_data.c
	arm-linux-gcc -finput-charset=GBK -fexec-charset=GBK -c -o show_font.o  ../03th_lcd_angle/show_font.c -lfreetype  -lm  
	arm-linux-gcc 											-o ft_show_font  show_font.o  font_data.o	  -lfreetype  -lm
	rm -rf *.o	

#all:
#	arm-linux-gcc -c -o  font_data.o  ../02th_lcd/font_data.c
#	arm-linux-gcc -finput-charset=GBK -fexec-charset=GBK -c -o show_font.o  ../02th_lcd/show_font.c -lfreetype  -lm  
#	arm-linux-gcc -o ft_show_font     show_font.o font_data.o	-lfreetype
#	rm -rf *.o	

#all: 
#	arm-linux-gcc -finput-charset=GBK  -o  example  ../01th/example1.c  -lfreetype -lm 

#all: 
#	arm-linux-gcc -o freetype_arm ../01th/freetype_arm.c
	
clean:
	rm-rf *.o	