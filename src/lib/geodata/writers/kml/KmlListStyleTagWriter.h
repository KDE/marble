//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mayank Madan <maddiemadan@gmail.com>
//

#ifndef MARBLE_KMLLISTSTYLETAGWRITER_H
#define MARBLE_KMLLISTSTYLETAGWRITER_H

#include "GeoTagWriter.h"
#include "GeoDataListStyle.h"

namespace Marble
{

class KmlListStyleTagWriter : public GeoTagWriter
{
public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;

private:
    QString itemTypeToString( GeoDataListStyle::ListItemType itemType ) const;
    QString iconStateToString( GeoDataItemIcon::ItemIconStates state ) const;
};

}

#endif
