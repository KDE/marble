//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#include "OsmFeatureTagTranslator.h"


namespace Marble
{
OsmFeatureTagTranslator::OsmFeatureTagTranslator() {
//nothing to do
}

bool OsmFeatureTagTranslator::write( const Marble::GeoNode *node, GeoWriter &writer ) const
{
    // Write identifiers
    bool const result = writeMid( node, writer );
    return result;
}

}
