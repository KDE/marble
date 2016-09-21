//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMDOCUMENTTAGTRANSLATOR_H
#define MARBLE_OSMDOCUMENTTAGTRANSLATOR_H

#include "GeoTagWriter.h"

namespace Marble
{
/**
     This helper class organizes a GeoDataDocument to
     meet the following OSM standard structure:

     <xml>
     <bounds>

     block of nodes: ( <node>...) 1st
     block of ways:  ( <way> ...) 2nd
     block of relations: ( <relation> ...) 3rd

     See http://wiki.openstreetmap.org/wiki/OSM_XML#Contents

*/

class OsmDocumentTagTranslator : public GeoTagWriter
{
public:
  bool write( const GeoNode *node, GeoWriter& writer ) const;
};

}

#endif

