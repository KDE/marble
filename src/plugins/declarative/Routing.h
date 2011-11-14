//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_ROUTING_H
#define MARBLE_DECLARATIVE_ROUTING_H

#include <QtCore/QObject>
#include <QtDeclarative/QtDeclarative>

class QAbstractItemModel;

namespace Marble
{

class MarbleWidget;

namespace Declarative
{

class RoutingPrivate;

class Routing : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString routingProfile READ routingProfile WRITE setRoutingProfile NOTIFY routingProfileChanged)

public:
    enum RoutingProfile { Motorcar, Bicycle, Pedestrian };

    explicit Routing( QObject* parent = 0 );

    ~Routing();

    void setMarbleWidget( Marble::MarbleWidget* widget );

    QString routingProfile() const;

    void setRoutingProfile( const QString & profile );

public Q_SLOTS:
    void addVia( qreal lon, qreal lat );

    void setVia( int index, qreal lon, qreal lat );

    void removeVia( int index );

    void clearRoute();

    void updateRoute();

    QObject* waypointModel();

    QObject* routeRequestModel();

Q_SIGNALS:
    void routingProfileChanged();

private:
    RoutingPrivate* const d;
};

}

}

#endif
