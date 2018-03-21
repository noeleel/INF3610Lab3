///////////////////////////////////////////////////////////////////////////////
//
//	main.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include <systemc.h>
#include "Sobel.h"
#include "Sobelv2.h"
#include "Reader.h"
#include "DataRAM.h"
#include "CacheMem.h"
#include "Writer.h"

#define RAMSIZE 0x200000

///////////////////////////////////////////////////////////////////////////////
//
//	Main
//
///////////////////////////////////////////////////////////////////////////////
int sc_main(int arg_count, char **arg_value)
{
	// Variables
	int sim_units = 2; //SC_NS

	// Clock
	const sc_core::sc_time_unit simTimeUnit = SC_NS;
	const int clk_freq = 4000;
	sc_clock clk("SysClock", clk_freq, simTimeUnit, 0.5);

	// Components
	//TODO : Déclaration des modules
	Reader reader("reader");
	Writer writer("writer");
	DataRAM dataRAM("dataRAM", "image.mem", RAMSIZE, false);

	// Signals
	sc_signal<unsigned int, SC_MANY_WRITERS> data;
	sc_signal<unsigned int, SC_MANY_WRITERS> address;
	sc_signal<unsigned int*> addressData;
	sc_signal<unsigned int> length;
	sc_signal<bool, SC_MANY_WRITERS> reqRead;
	sc_signal<bool, SC_MANY_WRITERS> ackReaderWriter;
	// ...
	sc_signal<bool, SC_MANY_WRITERS> reqWrite;
	sc_signal<bool, SC_MANY_WRITERS> reqCache;
	sc_signal<bool, SC_MANY_WRITERS> ackCache;

	// Connexions
	// TODO: Ajouter connexions 
	reader.clk(clk);
	reader.dataPortRAM(dataRAM);
	reader.data(data);
	reader.address(address);
	reader.ack(ackReaderWriter);
	reader.request(reqRead);

	writer.clk(clk);
	writer.dataPortRAM(dataRAM);
	writer.data(data);
	writer.address(address);
	writer.ack(ackReaderWriter);
	writer.request(reqWrite);

	//const bool utiliseCacheMem = false;
	const bool utiliseCacheMem = true;

	if (!utiliseCacheMem) {
		Sobel sobel("Sobel");

		/* à compléter (connexion ports)*/
		sobel.clk(clk);
		sobel.address(address);
		sobel.data(data);
		sobel.requestRead(reqRead);
		sobel.requestWrite(reqWrite);
		sobel.ack(ackReaderWriter);
		// Démarrage de l'application
		cout << "Démarrage de la simulation." << endl;
		sc_start(-1, sc_core::sc_time_unit(sim_units));
		cout << endl << "Simulation s'est terminée à " << sc_time_stamp();
	} else {
		Sobelv2 sobel("Sobel");
		CacheMem cacheMem("CacheMem");

		/* à compléter (connexion ports)*/
		sobel.clk(clk);
		sobel.address(address);
		sobel.dataRW(data);
		sobel.requestRead(reqRead);
		sobel.requestWrite(reqWrite);
		sobel.ackReaderWriter(ackReaderWriter);
		sobel.addressRes(addressData);
		sobel.length(length);
		sobel.requestCache(reqCache);
		sobel.ackCache(ackCache);

		cacheMem.clk(clk);
		cacheMem.addressData(addressData);
		cacheMem.length(length);
		cacheMem.requestFromCPU(reqCache);
		cacheMem.ackToCPU(ackCache);
		cacheMem.address(address);
		cacheMem.dataReader(data);
		cacheMem.requestToReader(reqRead);
		cacheMem.ackFromReader(ackReaderWriter);
	
		// Démarrage de l'application
		cout << "Démarrage de la simulation." << endl;
		sc_start(-1, sc_core::sc_time_unit(sim_units));
		cout << endl << "Simulation s'est terminée à " << sc_time_stamp();

	}

	return 0;
}
