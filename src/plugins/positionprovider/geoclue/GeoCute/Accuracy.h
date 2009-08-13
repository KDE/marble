#ifndef GEOCUTE_ACCURACY_H
#define GEOCUTE_ACCURACY_H

#include <QtCore/QMetaType>
#include <QtCore/QtGlobal>

#include "AccuracyLevel.h"


class QDBusArgument;

namespace GeoCute
{
    
class Accuracy
{
    public:
        AccuracyLevel level;
        qreal horizontal;
        qreal vertical;
};

QDBusArgument& operator<<(QDBusArgument& argument, const Accuracy& accuracy);
const QDBusArgument& operator>>(const QDBusArgument& argument,
    Accuracy& accuracy);

}

Q_DECLARE_METATYPE(GeoCute::Accuracy)



#endif
