//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// Description: Some quick and dirty substitutes for (trigonometric) 
// functions
//

#ifndef FASTMATH_H
#define FASTMATH_H


namespace FastMath {
    //	extern int ACos[32768];

    void cpuid( unsigned long , unsigned long&, unsigned long&, 
		unsigned long&, unsigned long& );
    bool haveSSE();

    //	inline const int acos_lt (int i) { return ACos[i]; }
    //	const int atan2_lt(double x, double y);
    //	extern ushort ATan2[16384];
}


#endif
