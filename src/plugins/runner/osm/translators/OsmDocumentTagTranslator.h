// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMDOCUMENTTAGTRANSLATOR_H
#define MARBLE_OSMDOCUMENTTAGTRANSLATOR_H

#include "GeoTagWriter.h"

namespace Marble
{
/**
     This helper class organizes a GeoDataDocument to
     meet the following OSM standard structure:
@code
     <xml>
     <bounds>

     block of nodes: ( <node>...) 1st
     block of ways:  ( <way> ...) 2nd
     block of relations: ( <relation> ...) 3rd
@endcode

     See https://wiki.openstreetmap.org/wiki/OSM_XML#Contents

*/

class OsmDocumentTagTranslator : public GeoTagWriter
{
public:
  bool write( const GeoNode *node, GeoWriter& writer ) const override;
};

}

#endif

