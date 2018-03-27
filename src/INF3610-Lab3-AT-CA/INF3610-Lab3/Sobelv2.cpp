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
	unsigned int imgWidth, imgHeight, data, tampon, readSize;
	unsigned int addr = 0;
	unsigned int addrRes = 0;
	while (true) {
		imgWidth = Read(addr);
		addr += 4;
		imgHeight = Read(addr);
		imgSize = imgWidth * imgHeight;
		tampon = imgWidth * 4;
		readSize = imgWidth * 3;
		addrRes = (unsigned int*)cache
		uint8_t* cache = new uint8_t[tampon];
		uint8_t* image = new uint8_t[imgSize];
		CacheRead(addr, addrRes, readSize);
		addr += readSize;
		
		int index = 0;
		bool isCacheRead = false;
		for (int i = 0; i < imgHeight; i++) {
			fCacheRead = false;
			for (int j = 0; j < imgWidth; j++) {
				index = i * imgWidth + j;
				if (i == 0 || j == 0 || i == imgHeight - 1 || j == imgWidth - 1) {
					image[index] = 0;
					Write(index + 8, 0);
				}
				else {
					if(!fCacheRead) {
						addrRes = (unsigned int*)cache + (((readSize + (i * imgWidth) - 4) % tampon) / 4);
						CacheRead(addr, addrRes, imgWidth);
						addr += imgWidth;
						fCacheRead = true;
					}
					image[index] = Sobelv2_operator(((addr - readSize - 8) % tampon) + j, imgWidth, cache);
					wait(clk->posedge_event());
				}
			}
		}

		for (int i = 0; i < imgHeight; i++) {
			for (int j = 0; j < imgWidth; j += 4) {
				index = i * imgWidth + j;
				data = (image[index] << 24 | image[index + 1] << 16| image[index + 2] << 8 | image[index + 3])
				Write(index + 8, data));
			}
		}

		delete cache;
		delete image;
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
