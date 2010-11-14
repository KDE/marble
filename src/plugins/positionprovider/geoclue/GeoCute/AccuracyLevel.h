//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_ACCURACYLEVEL_H
#define GEOCUTE_ACCURACYLEVEL_H



namespace GeoCute
{

enum AccuracyLevel {
    AccuracyLevelNone = 0,
    AccuracyLevelCountry,
    AccuracyLevelRegion,
    AccuracyLevelLocality,
    AccuracyLevelPostalCode,
    AccuracyLevelStreet,
    AccuracyLevelDetailed
};

}



#endif
