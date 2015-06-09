//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Niko Sams <niko.sams@gmail.com>
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
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
