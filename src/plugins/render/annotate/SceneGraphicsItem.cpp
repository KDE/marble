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

// Self
#include "SceneGraphicsItem.h"

// Marble
#include "GeoDataPlacemark.h"
#include "GeoDataGeometry.h"


namespace Marble
{

SceneGraphicsItem::SceneGraphicsItem( GeoDataPlacemark *placemark ) :
    GeoGraphicsItem( placemark ),
    m_state( Editing ),
    m_hasFocus( false ),
    m_request( NoRequest ),
    m_placemark( placemark )
{
    // nothing to do
}

SceneGraphicsItem::~SceneGraphicsItem()
{
    // nothing to do
}

const GeoDataLatLonAltBox &SceneGraphicsItem::latLonAltBox() const
{
    return m_placemark->geometry()->latLonAltBox();
}

SceneGraphicsItem::ActionState SceneGraphicsItem::state() const
{
    return m_state;
}

void SceneGraphicsItem::setState( ActionState state )
{
    ActionState previousState = m_state;
    m_state = state;
    dealWithStateChange( previousState );
}

bool SceneGraphicsItem::hasFocus() const
{
    return m_hasFocus;
}

void SceneGraphicsItem::setFocus( bool enabled )
{
    m_hasFocus = enabled;
}

SceneGraphicsItem::MarbleWidgetRequest SceneGraphicsItem::request() const
{
    return m_request;
}

void SceneGraphicsItem::setRequest( MarbleWidgetRequest request )
{
    m_request = request;
}

const GeoDataPlacemark *SceneGraphicsItem::placemark() const
{
    return m_placemark;
}

GeoDataPlacemark *SceneGraphicsItem::placemark()
{
    return m_placemark;
}

bool SceneGraphicsItem::sceneEvent( QEvent *event )
{
    if ( event->type() == QEvent::MouseButtonPress ) {
        return mousePressEvent( static_cast<QMouseEvent*>( event ) );
    } else if ( event->type() == QEvent::MouseMove ) {
        return mouseMoveEvent( static_cast<QMouseEvent*>( event ) );
    } else if ( event->type() == QEvent::MouseButtonRelease ) {
        return mouseReleaseEvent( static_cast<QMouseEvent*>( event ) );
    }

    return false;
}

}

