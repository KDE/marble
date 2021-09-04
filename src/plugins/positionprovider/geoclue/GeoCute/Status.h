//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
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
