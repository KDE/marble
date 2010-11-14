//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_STATUS_H
#define GEOCUTE_STATUS_H



namespace GeoCute
{

enum Status {
    StatusError = 0,
    StatusUnavailable,
    StatusAcquiring,
    StatusAvailable
};

}



#endif
