//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef GEOWIDGETBUBBLE_H
#define GEOWIDGETBUBBLE_H

#include <QtCore/QString>
#include <QtGui/QWidget>

namespace Marble
{

class GeoPainter;
class ViewportParams;
class GeoSceneLayer;


class GeoWidgetBubble
{
public:
    GeoWidgetBubble();

    void paint(GeoPainter* p, ViewportParams* v, const QString& renderPos,
               GeoSceneLayer* l);

    void setGeoWidget( QWidget* w );
    QWidget* getGeoWidget();

    void initaliseMarbleWidget( QWidget* parent );
    bool marbleWidgetIsInitalised() const;

    void moveTo( QPoint screenPos );
    void setHidden( bool hide );
    bool isHidden() const;

private:
    QWidget* m_widget;
    bool m_hidden;
    bool marbleWidgetInitalised;
    QPoint m_offset;
    QPoint m_screenPosition;


};

}

#endif // GEOWIDGETBUBBLE_H
