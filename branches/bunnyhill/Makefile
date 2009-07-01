# Makefile Bunny Hill

CC = g++
BIN = bh
OBJ = main.o ogl.o textures.o mathlib.o collision.o player.o \
course.o common.o physics.o hud.o view.o env.o loop.o \
models.o treegen.o quadtree.o tools.o info.o sound.o \
tux.o keyframe.o toolkeys.o particles.o trackmarks.o sp.o \
config.o image.o race.o font.o gllib.o c_test.o gui_menu.o \
gui_elements.o ft_font.o  

CFLAGS = -Wall -O2 -I/usr/include/freetype2
LDFLAGS = -lglut -lGL -lGLU -lSDL -lSDL_image -lSDL_mixer -lfreetype 

$(BIN) : $(OBJ)
	g++ -o $(BIN) $(OBJ) $(LDFLAGS) $(CFLAGS)

# mmmm.o : mmmm.cpp mmmm.h
#	$(CC) -c mmmm.cpp $(CFLAGS)

ft_font.o : ft_font.cpp ft_font.h
	$(CC) -c ft_font.cpp $(CFLAGS)

sp.o : sp.cpp sp.h
	$(CC) -c sp.cpp $(CFLAGS)

player.o : player.cpp player.h
	$(CC) -c player.cpp $(CFLAGS)

sound.o : sound.cpp sound.h
	$(CC) -c sound.cpp $(CFLAGS)

gui_elements.o : gui_elements.cpp gui_elements.h
	$(CC) -c gui_elements.cpp $(CFLAGS)

gui_menu.o : gui_menu.cpp gui_menu.h
	$(CC) -c gui_menu.cpp $(CFLAGS)

c_test.o : c_test.cpp c_test.h
	$(CC) -c c_test.cpp $(CFLAGS)

gllib.o : gllib.cpp gllib.h
	$(CC) -c gllib.cpp $(CFLAGS)

info.o : info.cpp info.h
	$(CC) -c info.cpp $(CFLAGS)

font.o : font.cpp font.h
	$(CC) -c font.cpp $(CFLAGS)

race.o : race.cpp race.h
	$(CC) -c race.cpp $(CFLAGS)

image.o : image.cpp image.h
	$(CC) -c image.cpp $(CFLAGS)

config.o : config.cpp config.h
	$(CC) -c config.cpp $(CFLAGS)

trackmarks.o : trackmarks.cpp trackmarks.h
	$(CC) -c trackmarks.cpp $(CFLAGS)

particles.o : particles.cpp particles.h
	$(CC) -c particles.cpp $(CFLAGS)

toolkeys.o : toolkeys.cpp toolkeys.h
	$(CC) -c toolkeys.cpp $(CFLAGS)

keyframe.o : keyframe.cpp keyframe.h
	$(CC) -c keyframe.cpp $(CFLAGS)

tux.o : tux.cpp tux.h
	$(CC) -c tux.cpp $(CFLAGS)

tools.o : tools.cpp tools.h
	$(CC) -c tools.cpp $(CFLAGS)

quadtree.o : quadtree.cpp quadtree.h
	$(CC) -c quadtree.cpp $(CFLAGS)

treegen.o : treegen.cpp treegen.h
	$(CC) -c treegen.cpp $(CFLAGS)

models.o : models.cpp models.h
	$(CC) -c models.cpp $(CFLAGS)

collision.o : collision.cpp collision.h
	$(CC) -c collision.cpp $(CFLAGS)

imagex.o : imagex.cpp imagex.h
	$(CC) -c imagex.cpp $(CFLAGS)

env.o : env.cpp env.h
	$(CC) -c env.cpp $(CFLAGS)

common.o : common.cpp common.h
	$(CC) -c common.cpp $(CFLAGS)

view.o : view.cpp view.h
	$(CC) -c view.cpp $(CFLAGS)

hud.o : hud.cpp hud.h
	$(CC) -c hud.cpp $(CFLAGS)

physics.o : physics.cpp physics.h
	$(CC) -c physics.cpp $(CFLAGS)

course.o : course.cpp course.h
	$(CC) -c course.cpp $(CFLAGS)

loop.o : loop.cpp loop.h
	$(CC) -c loop.cpp $(CFLAGS)

mathlib.o : mathlib.cpp mathlib.h
	$(CC) -c mathlib.cpp $(CFLAGS)

textures.o : textures.cpp textures.h
	$(CC) -c textures.cpp $(CFLAGS)

ogl.o : ogl.cpp ogl.h
	$(CC) -c ogl.cpp $(CFLAGS)

main.o : main.cpp bh.h 
	$(CC) -c main.cpp $(CFLAGS)

