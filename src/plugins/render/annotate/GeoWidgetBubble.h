//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
// Copyright 2013      Thibaut Gridel <tgridel@free.fr>
//

#ifndef GEOWIDGETBUBBLE_H
#define GEOWIDGETBUBBLE_H

#include <QString>
#include <QWidget>

namespace Marble
{

class GeoDataPlacemark;
class GeoSceneLayer;

class GeoWidgetBubble
{
public:
    explicit GeoWidgetBubble( GeoDataPlacemark *placemark );
    ~GeoWidgetBubble();

    void paint(QPainter* p);

    void setParentWidget( QWidget* parent );

    void moveTo( const QPoint &screenPos );
    void setHidden( bool hide );
    bool isHidden() const;

private:
    QWidget* m_widget;
    bool m_hidden;
    bool m_widgetInitialized;
    QPoint m_offset;
    QPoint m_screenPosition;


};

}

#endif // GEOWIDGETBUBBLE_H
