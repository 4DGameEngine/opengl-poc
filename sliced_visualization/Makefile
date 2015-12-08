# Sample Maekfile
# Change whatever fields necessary
# __DON'T RUN THIS SCRIPT WITH sudo!!!__
# Detects if running on Darwin or Linux.

CXX=clang++
LD=clang++
WARNINGS=-pedantic -Wall -Werror -Wfatal-errors -Wextra -Wno-unused-parameter -Wno-unused-variable
CXXFLAGS=-std=c++1y -stdlib=libc++ -g -c
LDFLAGS=-std=c++1y -stdlib=libc++ -lc++abi
OGL_LIBRARY_PATH=$(HOME)/opengl/lib
OGL_INCLUDE_PATH=$(HOME)/opengl/include
OGL_STATIC_LIBS=$(OGL_LIBRARY_PATH)/libGLEW.a $(OGL_LIBRARY_PATH)/libsoil2.a
OGL_DYNAMIC_LIBS=-lglfw -lCGAL -lCGAL_Core -lgmp
OUTFILE=sliced
OUTFILE_DEMO=demo

UNAME_S=$(shell uname -s)
ifeq ($(UNAME_S),Linux)
	OGL_EXTRA_LIBS=-lGL -lX11 -lpthread -lXrandr -lXi 
	CXXFLAGS+=-D__LINUXOS__
endif
ifeq ($(UNAME_S),Darwin)
	OGL_EXTRA_LIBS=-framework OpenGL
	CXXFLAGS+=-D__APPLEOS__
endif
	
all: build run

all_demo: demo run_demo

demo: demo.o shader.o camera.o utils4D.o object4D.o
	$(LD) $^ -o $(OUTFILE_DEMO) -L$(OGL_LIBRARY_PATH) $(LDFLAGS) -Wl,-rpath $(OGL_LIBRARY_PATH) $(OGL_STATIC_LIBS) $(OGL_DYNAMIC_LIBS) $(OGL_EXTRA_LIBS)

build: main.o shader.o camera.o utils4D.o object4D.o
	$(LD) $^ -o $(OUTFILE) -L$(OGL_LIBRARY_PATH) $(LDFLAGS) -Wl,-rpath $(OGL_LIBRARY_PATH) $(OGL_STATIC_LIBS) $(OGL_DYNAMIC_LIBS) $(OGL_EXTRA_LIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(WARNINGS) -I$(OGL_INCLUDE_PATH) $<

shader.o: shader.cpp shader.h
	$(CXX) $(CXXFLAGS) $(WARNINGS) -I$(OGL_INCLUDE_PATH) $<

camera.o: camera.cpp camera.h
	$(CXX) $(CXXFLAGS) $(WARNINGS) -I$(OGL_INCLUDE_PATH) $<

utils4D.o: utils4D.cpp utils4D.h
	$(CXX) $(CXXFLAGS) $(WARNINGS) -I$(OGL_INCLUDE_PATH) $<

object4D.o: object4D.cpp object4D.h camera.h shader.h utils4D.h
	$(CXX) $(CXXFLAGS) $(WARNINGS) -I$(OGL_INCLUDE_PATH) $<

demo.o: demo.cpp
	$(CXX) $(CXXFLAGS) $(WARNINGS) -I$(OGL_INCLUDE_PATH) $<

.PHONY: run run_demo clean

run:
	./$(OUTFILE)

run_demo:
	./$(OUTFILE_DEMO)

clean:
	rm -rf *.o $(OUTFILE) $(OUTFILE_DEMO)
