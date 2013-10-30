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

#include <QObject>
#if QT_VERSION < 0x050000
  #include <QtDeclarative/qdeclarative.h>
#else
  #include <QtQml/qqml.h>
#endif

class QAbstractItemModel;

class MarbleWidget;
class RoutingPrivate;

class Routing : public QObject
{
    Q_OBJECT
    Q_PROPERTY( MarbleWidget* map READ map WRITE setMap NOTIFY mapChanged)
    Q_PROPERTY( QString routingProfile READ routingProfile WRITE setRoutingProfile NOTIFY routingProfileChanged )
    Q_PROPERTY( bool hasRoute READ hasRoute NOTIFY hasRouteChanged )

public:
    enum RoutingProfile { Motorcar, Bicycle, Pedestrian };

    explicit Routing( QObject* parent = 0 );

    ~Routing();

    void setMap( MarbleWidget* widget );

    MarbleWidget *map();

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
    void mapChanged();

    void routingProfileChanged();

    void hasRouteChanged();

private:
    RoutingPrivate* const d;
};

#endif
