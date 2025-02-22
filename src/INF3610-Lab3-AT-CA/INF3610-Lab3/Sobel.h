///////////////////////////////////////////////////////////////////////////////
//
//	Sobel.h
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <systemc.h>
#include <stdio.h>


///////////////////////////////////////////////////////////////////////////////
//
//	Class Sobel
//
///////////////////////////////////////////////////////////////////////////////
class Sobel : public sc_module
{
	public: 
		// Ports
		sc_in_clk				clk;
		sc_inout<unsigned int>  data;
		sc_out<unsigned int>  	address;
		sc_out<bool>			requestRead;
		sc_out<bool>			requestWrite;
		sc_in<bool>				ack;

	
		// Constructor
		Sobel( sc_module_name name );
		
		// Destructor
		~Sobel();
		
	private:
		// Process SystemC
		/* └ complÚter */
		SC_HAS_PROCESS(Sobel);

		
		void thread(void);
		unsigned int Sobel::Read(const unsigned int addr);
		void Sobel::Write(const unsigned int addr, const unsigned int writeData);
		uint8_t Sobel::Sobel_operator(const int index, const int imgWidth, uint8_t * image);
};
