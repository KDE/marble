//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_ROUTING_H
#define MARBLE_DECLARATIVE_ROUTING_H

#include <QObject>
#include <QtQml/qqml.h>
#include <QQuickPaintedItem>

class QAbstractItemModel;

namespace Marble {

class MarbleMap;
class RoutingPrivate;

class Routing : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY( MarbleMap* marbleMap READ marbleMap WRITE setMarbleMap NOTIFY marbleMapChanged)
    Q_PROPERTY( QString routingProfile READ routingProfile WRITE setRoutingProfile NOTIFY routingProfileChanged )
    Q_PROPERTY( bool hasRoute READ hasRoute NOTIFY hasRouteChanged )

public:
    enum RoutingProfile { Motorcar, Bicycle, Pedestrian };

    explicit Routing( QQuickItem* parent = 0 );

    ~Routing();

    // Implements QQuickPaintedItem interface
    void paint(QPainter * painter);

public:
    void setMarbleMap( MarbleMap* marbleMap );

    MarbleMap *marbleMap();

    QString routingProfile() const;

    void setRoutingProfile( const QString & profile );

    bool hasRoute() const;

public Q_SLOTS:
    void addVia( qreal lon, qreal lat );

    void setVia( int index, qreal lon, qreal lat );

    void removeVia( int index );

    void reverseRoute();

    void clearRoute();

    void updateRoute();

    void openRoute( const QString &filename );

    void saveRoute( const QString &filename );

    QObject* waypointModel();

Q_SIGNALS:
    void marbleMapChanged();

    void routingProfileChanged();

    void hasRouteChanged();

private:
    RoutingPrivate* const d;
};

}

#endif
