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

#ifndef COMPASS_FLOAT_ITEM_H
#define COMPASS_FLOAT_ITEM_H

#include <QtCore/QObject>

#include "GeoDataLatLonAltBox.h"
#include "MarbleAbstractFloatItem.h"

class QSvgRenderer;

/**
 * @short The class that creates a compass
 *
 */

class MarbleCompassFloatItem : public QObject, public MarbleAbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( MarbleLayerInterface )

 public:
    explicit MarbleCompassFloatItem( const QPointF &point = QPointF( 200.0, 10.0 ),
                                const QSizeF &size = QSizeF( 86.0, 86.0 ) );
    ~MarbleCompassFloatItem();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;


    void initialize ();

    bool isInitialized () const;

    QPainterPath backgroundShape() const;

    bool needsUpdate( ViewportParams *viewport );

    bool renderFloatItem( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer = 0 );

 private:
    QSvgRenderer  *m_svgobj;
    QPixmap        m_compass;
};

#endif
