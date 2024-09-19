// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_POSITION_H
#define GEOCUTE_POSITION_H

#include <QDateTime>
#include <QFlags>

#include "Accuracy.h"

namespace GeoCute
{

enum PositionFieldFlag { PositionFieldNone = 0, PositionFieldLatitude = 1 << 0, PositionFieldLongitude = 1 << 1, PositionFieldAltitude = 1 << 2 };

Q_DECLARE_FLAGS(PositionFields, PositionFieldFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(PositionFields)

struct Position {
    Accuracy accuracy;
    qreal altitude;
    PositionFields fields;
    qreal latitude;
    qreal longitude;
    QDateTime timestamp;
};

}

#endif
