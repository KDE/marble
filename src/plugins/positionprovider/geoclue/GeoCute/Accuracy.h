//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef GEOCUTE_ACCURACY_H
#define GEOCUTE_ACCURACY_H

#include <QMetaType>
#include <QtGlobal>

#include "AccuracyLevel.h"


class QDBusArgument;

namespace GeoCute
{
    
class Accuracy
{
    public:
        /**
         * @brief Approximate descriptive accuracy.
         */
        AccuracyLevel level;
        /**
         * @brief Horizontal accuracy in meters.
         */
        qreal horizontal;
        /**
         * @brief Vertical accuracy in meters.
         */
        qreal vertical;
};

QDBusArgument& operator<<(QDBusArgument& argument, const Accuracy& accuracy);
const QDBusArgument& operator>>(const QDBusArgument& argument,
    Accuracy& accuracy);

}

Q_DECLARE_METATYPE(GeoCute::Accuracy)



#endif
