//
// C++ Interface: FastMath
//
// Description: Some quick and dirty substitutes for (trigonometric) 
// functions
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution

#ifndef FASTMATH_H
#define FASTMATH_H

/**
@author Torsten Rahn
*/

namespace FastMath {
    //	extern int ACos[32768];

    void cpuid( unsigned long , unsigned long&, unsigned long&, 
		unsigned long&, unsigned long& );
    bool haveSSE();

    //	inline const int acos_lt (int i) { return ACos[i]; }
    //	const int atan2_lt(float x, float y);
    //	extern ushort ATan2[16384];
}


#endif
