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

#include "GeoDataLatLonAltBox.h"
#include "MarbleAbstractFloatItem.h"

class QSvgRenderer;

/**
 * @short The class that creates an overview map.
 *
 */

class MarbleOverviewMap : public MarbleAbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( MarbleLayerInterface )

 public:
    explicit MarbleOverviewMap( const QPointF &point = QPointF( 10.5, 10.5 ),
                                const QSizeF &size = QSizeF( 166.0, 86.0 ) );
    ~MarbleOverviewMap();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;


    void initialize ();

    bool isInitialized () const;

    bool needsUpdate( ViewportParams *viewport );

    bool renderFloatItem( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer = 0 );

 private:
    QSvgRenderer  *m_svgobj;
    QPixmap        m_worldmap;

    GeoDataLatLonAltBox m_latLonAltBox;
    double m_centerLat;
    double m_centerLon;
};

#endif
