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

namespace Marble
{

/**
 * @short The class that creates a compass
 *
 */

class CompassFloatItem  : public MarbleAbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( Marble::MarbleLayerInterface )

 public:
    explicit CompassFloatItem ( const QPointF &point = QPointF( -1.0, 10.0 ),
                                const QSizeF &size = QSizeF( 75.0, 75.0 ) );
    ~CompassFloatItem ();

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
    Q_DISABLE_COPY( CompassFloatItem )

    QSvgRenderer  *m_svgobj;
    QPixmap        m_compass;

    /// allowed values: -1, 0, 1; default here: 0. FIXME: Declare enum
    int            m_polarity;
};

}

#endif
