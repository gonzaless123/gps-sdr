/*----------------------------------------------------------------------------------------------*/
/*! \file init.cpp
//
// FILENAME: init.cpp
//
// DESCRIPTION: Initializes the receiver software
//
// DEVELOPERS: Gregory W. Heckler (2003-2009)
//
// LICENSE TERMS: Copyright (c) Gregory W. Heckler 2009
//
// This file is part of the GPS Software Defined Radio (GPS-SDR)
//
// The GPS-SDR is free software; you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version. The GPS-SDR is distributed in the hope that
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// Note:  Comments within this file follow a syntax that is compatible with
//        DOXYGEN and are utilized for automated document extraction
//
// Reference:
*/
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
#include "includes.h"
//#include "fft.h"				//!< Fixed point FFT object
#include "fifo.h"				//!< Circular buffer for Importing IF data
#include "keyboard.h"			//!< Handle user input via keyboard
#include "channel.h"			//!< Tracking channels
#include "correlator.h"			//!< Correlator
#include "acquisition.h"		//!< Acquisition
#include "pvt.h"				//!< PVT solution
#include "ephemeris.h"			//!< Ephemeris decode
#include "telemetry.h"			//!< Serial/GUI telemetry
#include "commando.h"			//!< Command interface
#include "sv_select.h"			//!< Drives acquisition/reacquisition process
//#include "post_process.h"		//!< Run the receiver from a file
/*----------------------------------------------------------------------------------------------*/

/*! Print out command arguments to std_out */
/*----------------------------------------------------------------------------------------------*/
void usage(int32 argc, char* argv[])
{
	int32 lcv;

	fprintf(stderr, "\n");
	for(lcv = 0; lcv < argc; lcv++)
		fprintf(stderr, "%s ",argv[lcv]);
	fprintf(stderr, "\nusage: [-p] [-o] [-l] [-v]\n");
	fprintf(stderr, "[-p] <filename> use prerecorded data\n");
	fprintf(stderr, "[-o] <filename1> <filename2> do ocean reflection\n");
	fprintf(stderr, "[-c] log high rate channel data\n");
	fprintf(stderr, "[-l] log navigation data\n");
	fprintf(stderr, "[-d] <N> decimate logged nav data by this N factor\n");
	fprintf(stderr, "[-g] log google earth data\n");
	fprintf(stderr, "[-v] be verbose \n");
	fprintf(stderr, "[-n] ncurses OFF \n");
	fprintf(stderr, "[-gui] run receiver with the GUI app over a named pipe\n");
	fprintf(stderr, "[-ser] run receiver with the GUI app over a serial port\n");
	fprintf(stderr, "[-w] start receiver in warm start, using almanac and last good position\n");
	fprintf(stderr, "[-u] run receiver with usrp-gps as child process\n");
	fprintf(stderr, "\n");

	exit(1);
}
/*----------------------------------------------------------------------------------------------*/


/*! Print out command arguments to std_out */
/*----------------------------------------------------------------------------------------------*/
void echo_options()
{
	FILE *fp;

	/* Do some error checking */
	if(gopt.post_process)
	{
		fp = fopen(gopt.filename_direct,"r");
		if(fp == NULL)
		{
			printf("\nCould not open %s for reading\n\n",gopt.filename_direct);
			exit(1);
		}

	}

	if(gopt.ocean)
	{
		fp = fopen(gopt.filename_direct,"r");
		if(fp == NULL)
		{
			printf("\nCould not open %s for reading\n\n",gopt.filename_direct);
			exit(1);
		}

		fp = fopen(gopt.filename_reflected,"r");
		if(fp == NULL)
		{
			printf("\nCould not open %s for reading\n\n",gopt.filename_reflected);
			exit(1);
		}
	}


	fprintf(stderr, "\n");
	fprintf(stderr, "verbose:\t\t %d\n",gopt.verbose);
	fprintf(stderr, "realtime:\t\t %d\n",gopt.realtime);
	fprintf(stderr, "post_process:\t\t %d\n",gopt.post_process);
	fprintf(stderr, "ocean:\t\t\t %d\n",gopt.ocean);
	fprintf(stderr, "log_channel:\t\t %d\n",gopt.log_channel);
	fprintf(stderr, "log_nav:\t\t %d\n",gopt.log_nav);
	fprintf(stderr, "log_decimate:\t\t %d\n",gopt.log_decimate);
	fprintf(stderr, "google_earth:\t\t %d\n",gopt.google_earth);
	fprintf(stderr, "ncurses:\t\t %d\n",gopt.ncurses);
	fprintf(stderr, "gui:\t\t\t %d\n",gopt.gui);
	fprintf(stderr, "serial:\t\t\t %d\n",gopt.serial);
	fprintf(stderr, "filename_direct:\t %s\n",gopt.filename_direct);
	fprintf(stderr, "filename_reflected:\t %s\n",gopt.filename_reflected);
	fprintf(stderr, "\n");

}
/*----------------------------------------------------------------------------------------------*/


/*! Parse command line arguments to setup functionality */
/*----------------------------------------------------------------------------------------------*/
void Parse_Arguments(int32 argc, char* argv[])
{

	int32 lcv;

	/* Set default options */
	gopt.verbose 		= 0;
	gopt.realtime 		= 1;
	gopt.post_process 	= 0;
	gopt.ocean 			= 0;
	gopt.log_channel 	= 0;
	gopt.log_nav		= 0;
	gopt.log_decimate	= 1;
	gopt.google_earth	= 0;
	gopt.ncurses 		= 1;
	gopt.gui			= 0;
	gopt.serial			= 0;
	gopt.doppler_min 	= -MAX_DOPPLER_STRONG;
	gopt.doppler_max 	= MAX_DOPPLER_STRONG;
	gopt.corr_sleep 	= 500;
	gopt.startup		= 0;
	gopt.usrp_internal	= 0;
	strcpy(gopt.filename_direct, "data.bda");
	strcpy(gopt.filename_reflected, "rdata.bda");

	for(lcv = 1; lcv < argc; lcv++)
	{
		if(strcmp(argv[lcv],"-p") == 0)
		{
			gopt.post_process = 1;
			gopt.realtime = 0;
			gopt.ocean = 0;
			gopt.corr_sleep = 100;

			if(argc < lcv+2)
				usage(argc, argv);

			strcpy(gopt.filename_direct, argv[lcv+1]);
			lcv += 1;
		}
		else if(strcmp(argv[lcv],"-o") == 0)
		{
			gopt.post_process = 0;
			gopt.realtime = 0;
			gopt.ocean = 1;

			if(argc < lcv+3)
				usage(argc, argv);

			strcpy(gopt.filename_direct, argv[lcv+1]);
			strcpy(gopt.filename_reflected, argv[lcv+2]);
			lcv += 2;
		}
		else if(strcmp(argv[lcv],"-v") == 0)
		{
			gopt.verbose = 1;
		}
		else if(strcmp(argv[lcv],"-d") == 0)
		{
			if(isdigit(argv[lcv+1][0]))
			{
				lcv++;
				gopt.log_decimate = atoi(argv[lcv]);
			}
			else
			{
				usage(argc, argv);
			}
		}
		else if(strcmp(argv[lcv],"-c") == 0)
		{
			gopt.log_channel = 1;
		}
		else if(strcmp(argv[lcv],"-l") == 0)
		{
			gopt.log_nav = 1;
		}
		else if(strcmp(argv[lcv],"-g") == 0)
		{
			gopt.google_earth = 1;
		}
		else if(strcmp(argv[lcv],"-n") == 0)
		{
			gopt.ncurses = 0;
		}
		else if(strcmp(argv[lcv],"-w") == 0)
		{
			gopt.startup = 1;
		}
		else if(strcmp(argv[lcv],"-gui") == 0)
		{
			gopt.gui = 1;
			gopt.serial = 0;
			gopt.ncurses = 0;
		}
		else if(strcmp(argv[lcv],"-ser") == 0)
		{
			gopt.serial = 1;
			gopt.gui = 0;
			gopt.ncurses = 0;
			gopt.log_decimate = 10;
		}
		else if(strcmp(argv[lcv],"-u") == 0)
		{
			gopt.usrp_internal = 1;
		}
		else
			usage(argc, argv);
	}

	echo_options();

}
/*----------------------------------------------------------------------------------------------*/


/*! Initialize any hardware (for realtime mode) */
/*----------------------------------------------------------------------------------------------*/
int32 Hardware_Init(void)
{

	if(CPU_MMX())
	{
		if(gopt.verbose)
			printf("Detected MMX\n");
	}
	else
		return(false);

	if(CPU_SSE())
	{
		if(gopt.verbose)
			printf("Detected SSE\n");
	}
	else
		return(false);

	if(CPU_SSE2())
	{
		if(gopt.verbose)
			printf("Detected SSE2\n");
	}
	else
		return(false);

	if(CPU_SSE3())
	{
		if(gopt.verbose)
			printf("Detected SSE3\n");
	}

	if(CPU_SSSE3())
	{
		if(gopt.verbose)
			printf("Detected SSSE3\n");
	}

	if(CPU_SSE41())
	{
		if(gopt.verbose)
			printf("Detected SSE4.1\n");
	}

	if(CPU_SSE42())
	{
		if(gopt.verbose)
			printf("Detected SSE4.2\n");
	}

	return(1);

}
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
/*! Initialize all threaded objects and global variables */
int32 Object_Init(void)
{
	int32 lcv;
	int32 failed;

	/* Get start of receiver */
	gettimeofday(&starttime, NULL);

	/* Create Keyboard object to handle user input */
	pKeyboard = new Keyboard();

	/* Now do the hard work? */
	pAcquisition = new Acquisition(IF_SAMPLE_FREQUENCY, IF_FREQUENCY);

	/* Decode the almanac and ephemerides */
	pEphemeris = new Ephemeris;

	/* Get data from either the USRP or disk */
	pFIFO = new FIFO;

	pSV_Select = new SV_Select;

	for(lcv = 0; lcv < MAX_CHANNELS; lcv++)
		pChannels[lcv] = new Channel(lcv);

	pCorrelator = new Correlator();

	pTelemetry = new Telemetry();

	pCommando = new Commando();

	pPVT = new PVT();

//	if(gopt.post_process)
//		pPost_Process = new Post_Process(gopt.filename_direct);
//
//	pthread_mutex_init(&mAcq, NULL);
//	pthread_mutex_unlock(&mAcq);
//	int32 gAcq_high = false;
//
//	//if(gopt.verbose)
//	{
//		printf("Cleared Object Init\n");
//		fflush(stdout);
//	}
//
//	return(1);

}
/*----------------------------------------------------------------------------------------------*/




/*! Initialize all pipes */
/*----------------------------------------------------------------------------------------------*/
int32 Pipes_Init(void)
{
	int32 lcv;

	/* Create all of the pipes */
	pipe((int *)SVS_2_COR_P);
	pipe((int *)CHN_2_EPH_P);
	pipe((int *)PVT_2_TLM_P);
	pipe((int *)SVS_2_TLM_P);
	pipe((int *)EKF_2_TLM_P);
	pipe((int *)CMD_2_TLM_P);
	pipe((int *)ACQ_2_SVS_P);
	pipe((int *)EKF_2_SVS_P);
	pipe((int *)PVT_2_SVS_P);
	pipe((int *)TLM_2_CMD_P);
	pipe((int *)SVS_2_ACQ_P);
	pipe((int *)COR_2_ACQ_P);
	pipe((int *)ISRP_2_PVT_P);
	pipe((int *)ISRM_2_PVT_P);

	/* Setup some of the non-blocking pipes */
	fcntl(COR_2_ACQ_P[WRITE], F_SETFL, O_NONBLOCK);
	fcntl(EKF_2_SVS_P[WRITE], F_SETFL, O_NONBLOCK);
	fcntl(SVS_2_TLM_P[WRITE], F_SETFL, O_NONBLOCK);
	fcntl(PVT_2_SVS_P[WRITE], F_SETFL, O_NONBLOCK);
	fcntl(EKF_2_SVS_P[READ],  F_SETFL, O_NONBLOCK);
	fcntl(SVS_2_COR_P[READ],  F_SETFL, O_NONBLOCK);
	fcntl(SVS_2_TLM_P[READ],  F_SETFL, O_NONBLOCK);
	fcntl(PVT_2_TLM_P[READ],  F_SETFL, O_NONBLOCK);
	fcntl(SVS_2_TLM_P[READ],  F_SETFL, O_NONBLOCK);
	fcntl(EKF_2_TLM_P[READ],  F_SETFL, O_NONBLOCK);
	fcntl(CMD_2_TLM_P[READ],  F_SETFL, O_NONBLOCK);

	if(gopt.verbose)
	{
		printf("Cleared Pipes Init\n");
		fflush(stdout);
	}

	return(1);

}
/*----------------------------------------------------------------------------------------------*/


/*! Finally start up the threads */
/*----------------------------------------------------------------------------------------------*/
int32 Thread_Init(void)
{
	int32 lcv;

	/* Set the global run flag to true */
	grun = 0x1;

	/* Start the keyboard thread to handle user input from stdio */
	pKeyboard->Start();

	/* Startup the PVT sltn */
	pPVT->Start();

//	/* Start up the correlators */
	pCorrelator->Start();

	/* Start up the acquistion */
	pAcquisition->Start();

	/* Start up the ephemeris */
	pEphemeris->Start();

	/* Start up the command interface */
	pCommando->Start();

	/* Start the SV select thread */
	pSV_Select->Start();

	/* Last thing to do */
	pTelemetry->Start();

//	/* Do the post process */
//	if(gopt.post_process)
//		pPost_Process->Start();

	/* Start up the FIFO */
	pFIFO->Start();

	return(1);

}
/*----------------------------------------------------------------------------------------------*/
