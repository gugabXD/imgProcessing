# Makefile para Windows

PROG = res.exe
FONTES = main.c lib\SOIL2\image_DXT.c lib\SOIL2\image_helper.c lib\SOIL2\SOIL2.c lib\SOIL2\wfETC.c
OBJETOS = $(FONTES:.c=.o)
CFLAGS = -g -Wall -Iinclude #-g  # Todas as warnings, infos de debug
LDFLAGS = -Llib\GL -lfreeglut -lopengl32 -lglu32 -lm
CC = gcc

$(PROG): $(OBJETOS)
	gcc $(CFLAGS) $(OBJETOS) -o $@ $(LDFLAGS)

clean:
	-@ del $(OBJETOS) $(PROG)