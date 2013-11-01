//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Gaurav Gupta <1989.gaurav@googlemail.com>
// Copyright 2013      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "GeoDataAbstractView.h"

namespace Marble {

class GeoDataAbstractViewPrivate
{
public:
    GeoDataTimeSpan m_timeSpan;
    GeoDataTimeStamp m_timeStamp;
};

GeoDataAbstractView::GeoDataAbstractView() :
    d( new GeoDataAbstractViewPrivate )
{
    // nothing to do
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

}
