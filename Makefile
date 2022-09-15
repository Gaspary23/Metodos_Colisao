PROG = Triangulo
CC = g++
CPPFLAGS = -O3 -lGL -lGLU -lglut -lm -lstdc++

OBJ_DIR = obj
SRC_DIR = src

FONTES = Ponto.cpp Poligono.cpp Temporizador.cpp ListaDeCoresRGB.cpp
OBJ = $(addprefix $(OBJ_DIR)/, $(FONTES:.cpp=.o))

all: check_obj_dir obj_loop main_obj prog 

check_obj_dir: 
    ifeq ("$(wildcard $(OBJ_DIR))", "")
		@mkdir obj/
    endif

obj_loop: $(OBJ)	
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(CC) -c -o $@ $<

main_obj: 
	$(CC) -c -o $(OBJ_DIR)/PontosNoTriangulo.o PontosNoTriangulo.cpp

prog:
	$(CC) $(OBJ) $(OBJ_DIR)/PontosNoTriangulo.o $(CPPFLAGS) -o $(PROG)

clean:
	-@ rm -f $(OBJ) $(OBJ_DIR)/PontosNoTriangulo.o $(PROG)
