# Makefile para Linux e macOS

PROG = Triangulo
FONTES = Ponto.cpp Poligono.cpp Temporizador.cpp ListaDeCoresRGB.cpp PontosNoTriangulo.cpp Linha.cpp InterseccaoEntreTodasAsLinhas.cpp

OBJETOS = $(FONTES:.cpp=.o)
CPPFLAGS = -g -O3 -DGL_SILENCE_DEPRECATION # -Wall -g  # Todas as warnings, infos de debug

UNAME = `uname`

all: $(TARGET)
	-@make $(UNAME)

Darwin: $(OBJETOS)
#	g++ $(OBJETOS) -O3 -Wno-deprecated -framework OpenGL -framework Cocoa -framework GLUT -lm -o $(PROG)
	g++ $(OBJETOS) -O3 -framework OpenGL -framework Cocoa -framework GLUT -lm -o $(PROG)

Linux: $(OBJETOS)
	gcc $(OBJETOS) -O3 -lGL -lGLU -lglut -lm -lstdc++ -o $(PROG)

clean:
	-@ rm -f $(OBJETOS) $(PROG)
