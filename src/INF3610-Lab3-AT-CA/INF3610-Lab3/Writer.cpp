///////////////////////////////////////////////////////////////////////////////
//
//	Writer.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include "Writer.h"

///////////////////////////////////////////////////////////////////////////////
//
//	Constructor
//
///////////////////////////////////////////////////////////////////////////////
Writer::Writer(sc_module_name name) : sc_channel(name)
{
	SC_THREAD(thread);
	sensitive << clk.pos();
}

///////////////////////////////////////////////////////////////////////////////
//
//	Destructor
//
///////////////////////////////////////////////////////////////////////////////
Writer::~Writer()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//	Thread
//
///////////////////////////////////////////////////////////////////////////////
void Writer::thread(void)
{
	// Variable
	unsigned int uiAddress;
	unsigned int uiData;

	// Boucle infinie
	while (1)
	{
		// Verify request port
		do {
			wait(clk->posedge_event());
		} while (!request.read());

		uiAddress = address.read();
		uiData = data.read();
		dataPortRAM->Write(uiAddress, uiData);
		ack.write(true);

		wait(clk->posedge_event());
		ack.write(false);
	}
}