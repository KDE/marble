//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

//
// This class is a test plugin.
//

#ifndef MARBLEOVERVIEWMAP_H
#define MARBLEOVERVIEWMAP_H

#include <QtCore/QObject>

#include "GeoDataLatLonAltBox.h"
#include "AbstractFloatItem.h"

class QSvgRenderer;

namespace Marble
{

/**
 * @short The class that creates an overview map.
 *
 */

class OverviewMap : public AbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( OverviewMap )
    
 public:
    explicit OverviewMap( const QPointF &point = QPointF( 10.5, 10.5 ),
                          const QSizeF &size = QSizeF( 166.0, 86.0 ) );
    ~OverviewMap();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;


    void initialize ();

    bool isInitialized () const;

    void changeViewport( ViewportParams *viewport );

    void paintContent( GeoPainter *painter, ViewportParams *viewport,
                       const QString& renderPos, GeoSceneLayer * layer = 0 );

 protected:
    bool eventFilter( QObject *object, QEvent *e );

 private:
    void changeBackground( const QString& target );

    QString m_target;
    QSvgRenderer  *m_svgobj;
    QPixmap        m_worldmap;

    GeoDataLatLonAltBox m_latLonAltBox;
    qreal m_centerLat;
    qreal m_centerLon;
};

}

#endif
