//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

#include <declarative/MarbleQuickItem.h>

#ifndef MARBLE_MAPS_H
#define MARBLE_MAPS_H

namespace Marble {

class MarbleMaps : public MarbleQuickItem
{
    Q_OBJECT

    Q_PROPERTY( bool suspended READ isSuspended NOTIFY isSuspendedChanged )
    Q_PROPERTY(bool keepScreenOn READ keepScreenOn WRITE setKeepScreenOn NOTIFY keepScreenOnChanged)

public:
    explicit MarbleMaps(QQuickItem *parent = 0);

    bool isSuspended() const;

    bool keepScreenOn() const;

public Q_SLOTS:
    void setKeepScreenOn(bool keepScreenOn);

Q_SIGNALS:
    void isSuspendedChanged(bool isSuspended);

    void keepScreenOnChanged(bool keepScreenOn);

private Q_SLOTS:
    void handleApplicationStateChange(Qt::ApplicationState state);

private:
    bool m_suspended;
    bool m_keepScreenOn;
};

}

#endif
