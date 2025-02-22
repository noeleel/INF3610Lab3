///////////////////////////////////////////////////////////////////////////////
//
//	main.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include <systemc.h>
#include "Sobel.h"
#include "Reader.h"
#include "Writer.h"
#include "DataRAM.h"

#define RAMSIZE 0x200000

// Global variables
bool m_bError = false;

///////////////////////////////////////////////////////////////////////////////
//
//	Main
//
///////////////////////////////////////////////////////////////////////////////
int sc_main(int arg_count, char **arg_value)
{
	// Variables
	int sim_units = 2; //SC_NS 
	
	// Components
	// TODO : Fait
	DataRAM dataRAM("DataRAM", "image.mem", RAMSIZE , false) ;
	Reader reader("Reader");
	Writer writer("Writer");
	Sobel sobel("Sobel");


	// Connexions
	// TODO : Fait
	reader.dataPortRAM(dataRAM);

	writer.dataPortRAM(dataRAM);

	sobel.readPort(reader);
	sobel.writePort(writer);

	// D�marrage de l'application
	if (!m_bError)
	{
		cout << "D�marrage de la simulation." << endl;	
		sc_start( -1, sc_core::sc_time_unit(sim_units) );
		cout << endl << "Simulation s'est termin�e � " << sc_time_stamp();
	}
	// Fin du programme
	return 0;
}
