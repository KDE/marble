//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

#ifndef MAPSCALEFLOATITEM_H
#define MAPSCALEFLOATITEM_H

#include <QtCore/QObject>

#include "MarbleAbstractFloatItem.h"

/**
 * @short The class that creates a map scale.
 *
 */

class MapScaleFloatItem : public MarbleAbstractFloatItem
{
    Q_OBJECT

 public:
    explicit MapScaleFloatItem( const QPointF &point = QPointF( 10.5, -10.5 ),
                                const QSizeF &size = QSizeF( 0.0, 40.0 ) );
    ~MapScaleFloatItem();

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
    int   invScale() const            { return m_invScale; }
    void  setInvScale( int invScale ) { m_invScale = invScale; }

    int      m_radius;
    int      m_invScale;

    int      m_leftBarMargin;
    int      m_rightBarMargin;
    int      m_scaleBarWidth;
    int      m_scaleBarHeight;
    double   m_scaleBarKm;

    int      m_bestDivisor;
    int      m_pixelInterval;
    int      m_valueInterval;

    QString  m_unit;

    void calcScaleBar();
};

#endif // MAPSCALEFLOATITEM_H
