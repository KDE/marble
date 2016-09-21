//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Gaurav Gupta <1989.gaurav@googlemail.com>
// Copyright 2013      Dennis Nienhüser <nienhueser@kde.org>
//

#include "GeoDataAbstractView.h"

#include "GeoDataCamera.h"
#include "GeoDataLookAt.h"
#include "GeoDataTypes.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataTimeStamp.h"

namespace Marble {

class GeoDataAbstractViewPrivate
{
public:
    GeoDataAbstractViewPrivate();

    GeoDataTimeSpan m_timeSpan;
    GeoDataTimeStamp m_timeStamp;
    AltitudeMode m_altitudeMode;
};

GeoDataAbstractViewPrivate::GeoDataAbstractViewPrivate() :
    m_timeSpan(),
    m_timeStamp(),
    m_altitudeMode( ClampToGround )
{
    // do nothing
}

GeoDataAbstractView::GeoDataAbstractView() :
    d( new GeoDataAbstractViewPrivate() )
{
    // do nothing
}

GeoDataAbstractView::~GeoDataAbstractView()
{
    delete d;
}

GeoDataAbstractView::GeoDataAbstractView( const GeoDataAbstractView &other ) :
    GeoDataObject( other ),
    d( new GeoDataAbstractViewPrivate( *other.d ) )
{
    // nothing to do
}

GeoDataAbstractView &GeoDataAbstractView::operator =( const GeoDataAbstractView &other )
{
    GeoDataObject::operator=( other );
    *d = *other.d;
    return *this;
}

GeoDataCoordinates GeoDataAbstractView::coordinates() const
{
    if ( nodeType() == GeoDataTypes::GeoDataLookAtType) {
        const GeoDataLookAt *lookAt = static_cast<const GeoDataLookAt*>( this );
        if( lookAt ){
            return lookAt->coordinates();
        }
    }
    else if( nodeType() == GeoDataTypes::GeoDataCameraType ){
        const GeoDataCamera *camera = static_cast<const GeoDataCamera*>( this );
        if ( camera ){
            return camera->coordinates();
        }
    }
    return GeoDataCoordinates();
}


bool GeoDataAbstractView::equals(const GeoDataAbstractView &other) const
{
    return GeoDataObject::equals(other) &&
           d->m_timeSpan == other.d->m_timeSpan &&
           d->m_timeStamp == other.d->m_timeStamp &&
           d->m_altitudeMode == other.d->m_altitudeMode;
}

const GeoDataTimeSpan &GeoDataAbstractView::timeSpan() const
{
    return d->m_timeSpan;
}

GeoDataTimeSpan &GeoDataAbstractView::timeSpan()
{
    return d->m_timeSpan;
}

void GeoDataAbstractView::setTimeSpan( const GeoDataTimeSpan &timeSpan )
{
    d->m_timeSpan = timeSpan;
}

GeoDataTimeStamp &GeoDataAbstractView::timeStamp()
{
    return d->m_timeStamp;
}

const GeoDataTimeStamp &GeoDataAbstractView::timeStamp() const
{
    return d->m_timeStamp;
}

void GeoDataAbstractView::setTimeStamp( const GeoDataTimeStamp &timeStamp )
{
    d->m_timeStamp = timeStamp;
}

AltitudeMode GeoDataAbstractView::altitudeMode() const
{
    return d->m_altitudeMode;
}

void GeoDataAbstractView::setAltitudeMode(const AltitudeMode altitudeMode)
{
    d->m_altitudeMode = altitudeMode;
}

}
