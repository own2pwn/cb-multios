/*

Author: Jason Williams <jdw@cromulence.com>

Copyright (c) 2015 Cromulence LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
#define ENABLE_BUFFERED_PRINTF

extern "C"
{
#include <libcgc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <prng.h>
}

#include <cutil_list.h>
#include <cutil_string.h>
#include "simulation.h"
#include "ecm.h"
#include "tcm.h"
#include "common.h"

void cgc_RunSimulation( cgc_uint8_t *pSecretPage )
{
	cgc_CSimulation oSim;

	cgc_uint64_t ecmEquipmentID = *((cgc_uint64_t*)(pSecretPage));
	cgc_uint64_t tcmEquipmentID = *((cgc_uint64_t*)(pSecretPage+8));

#if 0 
	cgc_printf( "Equipment ID ECM: " );
	cgc_PrintHexBytes( pSecretPage, 8 );
	cgc_printf( "Equipment ID TCM: " );
	cgc_PrintHexBytes( pSecretPage+8, 8 );
#endif

	oSim.cgc_AddSimulationComponent( new cgc_CECM( oSim.cgc_GetLanBus(), ecmEquipmentID ) );
	oSim.cgc_AddSimulationComponent( new cgc_CTCM( oSim.cgc_GetLanBus(), tcmEquipmentID ) );

	oSim.cgc_SetupSimulation();

	// cgc_Run simulation
	oSim.cgc_RunSimulation();	
}

int __attribute__((fastcall)) main(int secret_page_i, char *unused[]) 
{
	void *secret_page = (void *)secret_page_i;

	cgc_uint32_t *pPageArray = (cgc_uint32_t*)secret_page;
	cgc_uint32_t ts = pPageArray[0] + pPageArray[1] + pPageArray[2] + pPageArray[3];
	cgc_uint32_t idx = 0;

	ts &= 0x7FFFF;

	cgc_printf( "START::TS=$d\n", ts+1452975600 );
	
	bool bDone = false;
	do
	{
		cgc_RunSimulation( (cgc_uint8_t*)secret_page );

		for (;;)
		{
			if ( idx >= 1024 )
				idx = 0;

			ts += pPageArray[idx];
			
			ts &= 0x7FFFF;

			idx++;

			cgc_printf( "cgc_Run another simulation (TS=$d) (y/n)? ", ts+1452975600 );
			CUtil::cgc_String sLine;

			sLine = cgc_ReadLine();

			if ( sLine[0] == 'n' )
			{
				bDone = true;
				break;
			}
			else if ( sLine[0] == 'y' )
			{
				bDone = false;
				break;
			}
		}	
	} while ( !bDone );

	return 0;
}
