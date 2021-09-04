// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Niko Sams <niko.sams@gmail.com>
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ROUTINGPROFILE_H
#define MARBLE_ROUTINGPROFILE_H

#include "marble_export.h"

#include <QString>
#include <QHash>
#include <QVariant>

namespace Marble
{

class MARBLE_EXPORT RoutingProfile {
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(TransportType transportType READ transportType WRITE setTransportType)

public:
    enum TransportType {
        Motorcar,
        Bicycle,
        Pedestrian
    };

    explicit RoutingProfile( const QString &name = QString() );

    QString name() const;

    void setName( const QString &name );

    const QHash<QString, QHash<QString, QVariant> >& pluginSettings() const;

    QHash<QString, QHash<QString, QVariant> >& pluginSettings();

    void setTransportType( TransportType transportType );

    TransportType transportType() const;

    bool operator==( const RoutingProfile &other ) const;

private:
    QString m_name;
    //icon
    QHash<QString, QHash<QString, QVariant> > m_pluginSettings;

    TransportType m_transportType;
};

}

#endif
