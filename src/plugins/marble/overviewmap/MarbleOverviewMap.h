//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

//
// This class is a test plugin.
//

#ifndef MARBLEOVERVIEWMAP_H
#define MARBLEOVERVIEWMAP_H

#include <QtCore/QObject>

#include "MarbleAbstractFloatItem.h"

/**
 * @short The class that creates an overview map.
 *
 */

class MarbleOverviewMap : public QObject, public MarbleAbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( MarbleLayerInterface )

 public:
    MarbleOverviewMap( const QPointF &point = QPointF( 10.0, 10.0 ), 
                       const QSizeF &size = QSizeF( 150.0, 50.0 ) );

    QStringList backendTypes() const;

    QString name() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;


    void initialize ();

    bool isInitialized () const;

    bool renderContent( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer = 0 );
};

#endif
