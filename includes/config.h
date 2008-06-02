/*! \file Config.h
	Default includes for each source file in the project.
*/
/************************************************************************************************
Copyright 2008 Gregory W Heckler

This file is part of the GPS Software Defined Radio (GPS-SDR)

The GPS-SDR is free software; you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GPS-SDR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along with GPS-SDR; if not,
write to the: 

Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
************************************************************************************************/


#ifndef CONFIG_H
#define CONFIG_H


/* The most important thing, the NUMBER OF CORRELATORS IN THE RECEIVER and the NUMBER OF CPUs */
/*----------------------------------------------------------------------------------------------*/
#define MAX_CHANNELS			(12)						//!< Number of channel objects
#define CPU_CORES				(2)							//!< 1 for a single core, 2 for a dual core system, etc
#define CORR_PER_CPU			(MAX_CHANNELS/CPU_CORES)	//!< Distribute them up evenly (this should be an INTEGER!)
#define MAX_ANTENNA				(1)							//!< The number of antennas

/*----------------------------------------------------------------------------------------------*/


/* What stuff to log */
/*----------------------------------------------------------------------------------------------*/
#define CHANNEL_DEBUG			(1)			//!< Log channel data to HD  
#define CHANNEL_INVESTIGATE		(0)			//!< Try to figure out Nav bug  
#define LOG_PSEUDO				(0)			//!< Pseudoranges  
#define LOG_SV					(0)			//!< SV Navigation data  
#define LOG_NAV					(1)			//!< Nav Output  
/*----------------------------------------------------------------------------------------------*/


/* Work on what kind of Data?? */
/*----------------------------------------------------------------------------------------------*/
#define USRP_RECORDER			(1)			//!< Data collected with USRP
#define GRS_RECORDER			(0)			//!< Data recorder with GIZMOS recorder 
 /*----------------------------------------------------------------------------------------------*/


/* MISC GPS Constants */ 
/*----------------------------------------------------------------------------------------------*/
#define NUM_CODES				(32)		//!< Number of CDMA codes
#define NUM_CODES_WAAS			(51)		//!< Number of CDMA codes + WAAS PRNs

/*! WAAS PRNs will be referred to internally as PRN 32...50 (zero indexed) */
#define NON_ALLOCATED_PRN		(36)		//!< A non-allocated PRN number, used to generate acquisition thresholds 
/*----------------------------------------------------------------------------------------------*/


/* Acquisition defines */
/*----------------------------------------------------------------------------------------------*/
#define	THRESH_STRONG			(1.5e7)	//!< Threshold for strong signal detection
#define	THRESH_MEDIUM			(1.0e7)		//!< Threshold for medium signal detection
#define	THRESH_WEAK				(1.0e7)		//!< Threshold for weak signal detection
#define MAX_DOPPLER				7000		//!< Set the maximum Doppler frequency
/*----------------------------------------------------------------------------------------------*/


/* Correlator  Defines */
/*----------------------------------------------------------------------------------------------*/
#define	CORR_DELAYS				(1)			//!< Number of delays to calculate (plus-minus)
#define CORR_SPACING			(.5)		//!< How far should the correlators be spaced
#define FRAME_SIZE_PLUS_2		(12)		//!< 10 words per frame, 12 = 10 + 2 
#define MEASUREMENT_INT			(100)		//!< Packets of ~1ms data
#define CODE_BINS				(100)		//!< Partial code offset bins code resolution -> 1 chip/X bins
#define CARRIER_SPACING			(10)		//!< Spacing of bins (Hz)
#define CARRIER_BINS			(MAX_DOPPLER/CARRIER_SPACING) //!< Number of pre-sampled carrier wipeoff bins
#define ICP_TICS				(5)			//!< Number of measurement ints (plus-minus) to calculate ICP, 
											//!< this cannot exceed TICS_PER_SECOND/2 !!!!

#define TICS_2_SECONDS			(MEASUREMENT_INT/1000)
#define TICS_PER_SECOND			(1000/MEASUREMENT_INT)
/*----------------------------------------------------------------------------------------------*/


/* Channel defines */
/*----------------------------------------------------------------------------------------------*/
#define CARRIER_AIDING			(1)			//!< Carrier aid the DLL
#define PLL_BN					(15)		//!< PLL Bandwidth 
#define FLL_BN					(10)		//!< FLL Bandwidth 
#define AGC_BITS				(5)			//!< AGC to this bit depth 
#define AGC_LOW					(4)
#define AGC_HIGH				(16)
#define	MEASUREMENT_DELAY		(10)		//!< Number of measurements that have to be marked as navigate to allow PVT
/*----------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------*/
#define FIFO_PRIORITY			(99)
#define CORR_PRIORITY			(98)
#define EPHEM_PRIORITY			(97)
#define PVT_PRIORITY			(96)
#define TRAK_PRIORITY			(95)
#define ACQ_PRIORITY			(70)
#define TELEM_PRIORITY			(92)
#define KEY_PRIORITY			(93)
/*----------------------------------------------------------------------------------------------*/


#endif
