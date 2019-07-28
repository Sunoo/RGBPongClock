RGB_LIB_DISTRIBUTION=/home/pi/rpi-rgb-led-matrix

all : RGBPongClock
CXXFLAGS=-Wall -O3 -g -Wextra -Wno-unused-parameter

RGBPongClock: RGBPongClock.cpp
	g++ -I$(RGB_LIB_DISTRIBUTION)/include $(CXXFLAGS) RGBPongClock.cpp -o $@ -L$(RGB_LIB_DISTRIBUTION)/lib -lrgbmatrix -lrt -lm -lpthread

clean:
	rm -f RGBPongClock
