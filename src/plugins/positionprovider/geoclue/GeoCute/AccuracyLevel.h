// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
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
