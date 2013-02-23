C_COMPILER=gcc
C_FLAGS= -O2 -g -c -Wall -fPIC

LD_FLAGS= -lglut  -lGLU -lGL


OBJS=u3dspv
C_SRCS= u3dspv.c u3dplayer.c

all: u3dspv u3dplayer

u3dspv: ccomp
	$(C_COMPILER) -shared -Wl $@.o -o lib$@.so $(LD_FLAGS)
	
u3dplayer: ccomp
	$(C_COMPILER) -Wall *.o -o $@ $(LD_FLAGS)
ccomp:
	$(C_COMPILER) $(C_SRCS) $(C_FLAGS)

