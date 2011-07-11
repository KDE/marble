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

class Routing : public QObject
{
    Q_OBJECT

public:
    explicit Routing( QObject* parent = 0 );

    void setMarbleWidget( Marble::MarbleWidget* widget );

public Q_SLOTS:
    void addVia( qreal lon, qreal lat );

    void setVia( int index, qreal lon, qreal lat );

    void removeVia( int index );

    void clearRoute();

    QObject* waypointModel();

    QObject* routeRequestModel();

private:
    Marble::MarbleWidget* m_marbleWidget;

    QAbstractItemModel* m_routeRequestModel;
};

}

}

#endif
