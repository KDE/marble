// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

#include <declarative/MarbleQuickItem.h>
#include <qqmlregistration.h>

#ifndef MARBLE_MAPS_H
#define MARBLE_MAPS_H

namespace Marble
{

class MarbleMaps : public MarbleQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool suspended READ isSuspended NOTIFY isSuspendedChanged)
    Q_PROPERTY(bool keepScreenOn READ keepScreenOn WRITE setKeepScreenOn NOTIFY keepScreenOnChanged)

public:
    explicit MarbleMaps(QQuickItem *parent = nullptr);

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
