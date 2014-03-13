//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Adrian Draghici <draghici.adrian.b@gmail.com>
//

#ifndef MARBLE_KMLLINKTAGWRITER_H
#define MARBLE_KMLLINKTAGWRITER_H

#include "GeoTagWriter.h"
#include "GeoDataLink.h"

namespace Marble
{

class KmlLinkTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;

    static QString refreshModeToString( GeoDataLink::RefreshMode refreshMode) ;

private:
    static QString viewRefreshModeToString( GeoDataLink::ViewRefreshMode);

};

}

#endif
