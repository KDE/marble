//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "FastMath.h"

/*
namespace FastMath {
    int ACos[32768];
    int ATan2[32768];
}
*/

void FastMath::cpuid( unsigned long function,
		      unsigned long& out_eax, unsigned long& out_ebx,
		      unsigned long& out_ecx, unsigned long& out_edx )
{
    asm("cpuid": "=a" ( out_eax ),
	         "=b" ( out_ebx ),
		 "=c" ( out_ecx ),
		 "=d" ( out_edx ) 
		 : "a" ( function )
	);
}


bool FastMath::haveSSE()
{
    unsigned long eax,ebx,edx,unused;

    cpuid( 1,eax,ebx,unused,edx );
    return ( edx & 0x2000000L ) != 0;
}
