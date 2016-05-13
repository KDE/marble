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

public:
    explicit MarbleMaps(QQuickItem *parent = 0);

    bool isSuspended() const;

Q_SIGNALS:
    void isSuspendedChanged(bool isSuspended);

private Q_SLOTS:
    void handleApplicationStateChange(Qt::ApplicationState state);

private:
    bool m_suspended;

};

}

#endif
