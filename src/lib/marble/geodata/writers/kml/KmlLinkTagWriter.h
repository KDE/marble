//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Adrian Draghici <draghici.adrian.b@gmail.com>
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
    bool write( const GeoNode *node, GeoWriter& writer ) const override;

    static QString refreshModeToString( GeoDataLink::RefreshMode refreshMode) ;

private:
    static QString viewRefreshModeToString( GeoDataLink::ViewRefreshMode);

};

}

#endif
