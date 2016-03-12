//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson  <g.real.ate@gmail.com>
// Copyright 2013      Thibaut Gridel <tgridel@free.fr>
// Copyright 2014      Calin Cruceru  <crucerucalincristian@gmail.com>
//

#ifndef PLACEMARKTEXTANNOTATION_H
#define PLACEMARKTEXTANNOTATION_H

#include <QColor>
#include "SceneGraphicsItem.h"

namespace Marble
{

class PlacemarkTextAnnotation : public SceneGraphicsItem
{
public:
    explicit PlacemarkTextAnnotation( GeoDataPlacemark *placemark );
    ~PlacemarkTextAnnotation();

    virtual void paint(GeoPainter *painter, const ViewportParams *viewport, const QString &layer);

    virtual bool containsPoint( const QPoint &eventPos ) const;

    virtual void dealWithItemChange( const SceneGraphicsItem *other );

    virtual void move( const GeoDataCoordinates &source, const GeoDataCoordinates &destination );

    /**
     * @brief Provides information for downcasting a SceneGraphicsItem.
     */
    virtual const char *graphicType() const;

    /**
     * @brief Real label color, which is being hidden when placemark has focus
     */
    QColor labelColor() const;

protected:
    virtual bool mousePressEvent( QMouseEvent *event );
    virtual bool mouseMoveEvent( QMouseEvent *event );
    virtual bool mouseReleaseEvent( QMouseEvent *event );

    virtual void dealWithStateChange( SceneGraphicsItem::ActionState previousState );

private:
    const ViewportParams *m_viewport;
    bool m_movingPlacemark;
    QColor m_labelColor;

    QRegion m_region;
};

}

#endif // PLACEMARKTEXTANNOTATION_H
