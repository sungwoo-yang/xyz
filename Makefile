CC = g++
CFLAGS = -O3 -Wall -Wextra -pedantic -std=c++17 -I image_convolution
LFLAGS = -lpthread
PRG = bin/image_convolution

all: ColorGrid.o ConvolutePNG.o ConvolutionType.o lodepng.o main.o
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(PRG) ColorGrid.o ConvolutePNG.o ConvolutionType.o lodepng.o main.o $(LFLAGS)

ColorGrid.o: image_convolution/ColorGrid.cpp
	$(CC) $(CFLAGS) -c image_convolution/ColorGrid.cpp

ConvolutePNG.o: image_convolution/ConvolutePNG.cpp
	$(CC) $(CFLAGS) -c image_convolution/ConvolutePNG.cpp

ConvolutionType.o: image_convolution/ConvolutionType.cpp
	$(CC) $(CFLAGS) -c image_convolution/ConvolutionType.cpp

lodepng.o: image_convolution/lodepng.cpp
	$(CC) $(CFLAGS) -c image_convolution/lodepng.cpp

main.o: image_convolution/main.cpp
	$(CC) $(CFLAGS) -c image_convolution/main.cpp

clean:
	rm -rf $(wildcard bin *.o)