///////////////////////////////////////////////////////////////////////////////
//
//	Sobel.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include "Sobel.h"


///////////////////////////////////////////////////////////////////////////////
//
//	Constructeur
//
///////////////////////////////////////////////////////////////////////////////
Sobel::Sobel( sc_module_name name ) : sc_module(name)
/* À compléter */
{
	/*
	
	À compléter
	
	*/
	SC_THREAD(thread);
	sensitive << clk.pos();
}


///////////////////////////////////////////////////////////////////////////////
//
//	Destructeur
//
///////////////////////////////////////////////////////////////////////////////
Sobel::~Sobel()
{
	/*
	
	À compléter
	
	*/
}

unsigned int Sobel::Read(const unsigned int addr) {
	int readData = 0;
	address.write(addr);
	requestRead.write(true);
	do {
		wait(clk->posedge_event());
	} while (!ack.read());
	readData = data.read();
	requestRead.write(false);
	return readData;
}

void Sobel::Write(const unsigned int addr, const unsigned int writeData) {
	address.write(addr);
	data.write(writeData);
	requestWrite.write(true);
	do {
		wait(clk->posedge_event());
	} while (!ack.read());
	requestWrite.write(false);
}

///////////////////////////////////////////////////////////////////////////////
//
//	thread
//
///////////////////////////////////////////////////////////////////////////////
void Sobel::thread(void)
{
	/*
	
	À compléter
	
	*/
	unsigned int address = 0;
	unsigned int imgWidth, imgHeight, imgSize, pixel, data;

	while (true) {
		imgWidth = Read(address);
		address += 4;
		imgHeight = Read(address);
		imgSize = imgWidth * imgHeight;
		uint8_t* image = new uint8_t[imgSize];
		uint8_t* result = new uint8_t[imgSize];
		int * imageAsInt = reinterpret_cast<int*>(image);
		int * resultAsInt = reinterpret_cast<int*>(result);

		int index = 0;
		address += 4;
		for (int i = 0; i < imgHeight; i++) {
			for (int j = 0; j < imgWidth; j += 4) {
				index = i * imgWidth + j;
				pixel = Read(address + index);
				image[index] = pixel & 0xFF;
				image[index + 1] = (pixel >> 8) & 0xFF;
				image[index + 2] = (pixel >> 16) & 0xFF;
				image[index + 3] = pixel >> 24;
			}
		}

		for (int i = 0; i < imgHeight; i++) {
			for (int j = 0; j < imgWidth; j += 4) {
				index = i * imgWidth + j;
				if (i == 0 || i == imgHeight - 1) {
					Write(address + index, 0);
				}
				else if (j == 0 || j  == imgWidth - 1) {
					Write(address + index, 0);
				}
				else {
					result[index] = Sobel_operator(index, imgWidth, image);
					result[index + 1] = Sobel_operator(index + 1, imgWidth, image) ;
					result[index + 2] = Sobel_operator(index + 2, imgWidth, image) ;
					result[index + 3] = Sobel_operator(index + 3, imgWidth, image) ;
					data = (result[index] << 24 | result[index + 1] << 16| result[index+ 2] << 8 | result[index + 3]);
					Write(address + index, data);
				}
				wait(clk->posedge_event());
			}
		}

		delete image;
		delete result;

		sc_stop();
		wait();
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	sobel_operator
//
///////////////////////////////////////////////////////////////////////////////
static inline uint8_t getVal(int index, int xDiff, int yDiff, int img_width, uint8_t * Y) 
{ 
	return Y[index + (yDiff * img_width) + xDiff]; 
};

uint8_t Sobel::Sobel_operator(const int index, const int imgWidth, uint8_t * image)
{
	int x_weight = 0;
	int y_weight = 0;

	unsigned edge_weight;
	uint8_t edge_val;

	const char x_op[3][3] = {	{ -1,0,1 },
								{ -2,0,2 },
								{ -1,0,1 } };

	const char y_op[3][3] = {	{ 1,2,1 },
								{ 0,0,0 },
								{ -1,-2,-1 } };

	//Compute approximation of the gradients in the X-Y direction
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
		// X direction gradient
		x_weight = x_weight + (getVal(index, i - 1, j - 1, imgWidth, image) * x_op[i][j]);

		// Y direction gradient
		y_weight = y_weight + (getVal(index, i - 1, j - 1, imgWidth, image) * y_op[i][j]);
		}
	}

	edge_weight = std::abs(x_weight) + std::abs(y_weight);

	edge_val = (255 - (uint8_t)(edge_weight));

	//Edge thresholding
	if (edge_val > 200)
		edge_val = 255;
	else if (edge_val < 100)
		edge_val = 0;

	return edge_val;
}


