///////////////////////////////////////////////////////////////////////////////
//
//	Sobelv2.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include "Sobelv2.h"


///////////////////////////////////////////////////////////////////////////////
//
//	Constructeur
//
///////////////////////////////////////////////////////////////////////////////
Sobelv2::Sobelv2(sc_module_name name) : sc_module(name)
/* À compléter */
{
	SC_THREAD(thread);
	sensitive << clk.pos();
}


///////////////////////////////////////////////////////////////////////////////
//
//	Destructeur
//
///////////////////////////////////////////////////////////////////////////////
Sobelv2::~Sobelv2()
{
	/*

	À compléter

	*/
}


///////////////////////////////////////////////////////////////////////////////
//
//	thread
//
///////////////////////////////////////////////////////////////////////////////
void Sobelv2::thread(void)
{
	/*
	À compléter
	*/
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int data = 0x00000000;
	uint8_t* image = NULL;
	uint8_t* cache = NULL;
	unsigned int addr = 8;

	while (true) {
		width = Read(0);
		height = Read(4);

		cache = new uint8_t[4 * width]();
		image = new uint8_t[height * width]();

		CacheRead(addr, (unsigned int*)cache, 3 * width);
		addr += 3 * width;

		for (unsigned int i = 0; i < height; i++) {
			if (i != 0 && i != height - 1) {
				CacheRead(addr, ((unsigned int*)cache) + ((addr - 8) % (4 * width) / 4), width);
				addr += width;
			}
			for (unsigned int j = 0; j < width; j++) {
				if (i == 0 || j == 0 || i == height - 1 || j == width - 1) {
					image[i*width + j] = 0;
				}
				else {
					wait(12); // On attend 12 cycles. Voir explication du temps de traitement dans Sobel.cpp.
					image[i*width + j] = Sobelv2_operator((addr - 8 - 3 * width) % (width * 4) + j, width, cache);
				}
			}
		}

		for (unsigned int i = 0; i < height; i++) {
			for (unsigned int j = 0; j < width; j += 4) {
				Write(8 + (i*width) + j, image[i*width + j] +
					(image[i*width + j + 1] << 8) +
					(image[i*width + j + 2] << 16) +
					(image[i*width + j + 3] << 24));
			}
		}

		delete[] cache;
		delete[] image;
		sc_stop();
		wait();
	}

}



///////////////////////////////////////////////////////////////////////////////
//
//	Sobelv2_operator
//
///////////////////////////////////////////////////////////////////////////////
static inline uint8_t getVal(int index, int xDiff, int yDiff, int img_width, uint8_t * Y)
{
	int fullIndex = (index + (yDiff * img_width)) + xDiff;
	if (fullIndex < 0)
	{
		//Cas ou on doit chercher la derniere ligne
		fullIndex += img_width * 4;
	}
	else if (fullIndex >= img_width * 4)
	{
		//Cas ou on doit aller chercher la premiere ligne
		fullIndex -= img_width * 4;
	}

	return Y[fullIndex];
};

uint8_t Sobelv2::Sobelv2_operator(const int index, const int imgWidth, uint8_t * image)
{
	int x_weight = 0;
	int y_weight = 0;

	unsigned edge_weight;
	uint8_t edge_val;

	const char x_op[3][3] = { { -1,0,1 },
	{ -2,0,2 },
	{ -1,0,1 } };

	const char y_op[3][3] = { { 1,2,1 },
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


unsigned int Sobelv2::Read(const unsigned int addr) {
	int readData = 0;
	address.write(addr);
	requestRead.write(true);
	do {
		wait(clk->posedge_event());
	} while (!ackReaderWriter.read());
	readData = dataRW.read();
	requestRead.write(false);
	return readData;
}

void Sobelv2::Write(const unsigned int addr, const unsigned int writeData) {
	address.write(addr);
	dataRW.write(writeData);
	requestWrite.write(true);
	do {
		wait(clk->posedge_event());
	} while (!ackReaderWriter.read());
	requestWrite.write(false);
}

void Sobelv2::CacheRead(const unsigned int addr, unsigned int* addressData, const unsigned int lgt) {
	address.write(addr);
	addressRes.write(addressData);
	length.write(lgt);
	requestCache.write(true);
	do {
		wait(clk->posedge_event());
	} while (!ackCache.read());
	requestCache.write(false);
}