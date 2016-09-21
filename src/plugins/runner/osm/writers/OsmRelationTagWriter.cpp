//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

//Self
#include "OsmRelationTagWriter.h"

//Marble
#include "OsmElementDictionary.h"
#include "OsmObjectAttributeWriter.h"
#include "OsmTagTagWriter.h"
#include "GeoDataLineString.h"
#include "GeoDataPolygon.h"
#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"
#include "GeoWriter.h"
#include "osm/OsmPlacemarkData.h"
#include "osm/OsmObjectManager.h"

namespace Marble
{

void OsmRelationTagWriter::writeMultipolygon( const GeoDataPolygon& polygon,
                                              const OsmPlacemarkData& osmData, GeoWriter& writer )
{
    writer.writeStartElement( osm::osmTag_relation );

    OsmObjectAttributeWriter::writeAttributes( osmData, writer );
    OsmTagTagWriter::writeTags( osmData, writer );

    writer.writeStartElement( osm::osmTag_member );
    QString memberId = QString::number( osmData.memberReference( -1 ).id() );
    writer.writeAttribute( "type", "way" );
    writer.writeAttribute( "ref", memberId );
    writer.writeAttribute( "role", "outer" );
    writer.writeEndElement();

    for ( int index = 0; index < polygon.innerBoundaries().size(); ++index ) {
        writer.writeStartElement( osm::osmTag_member );
        QString memberId = QString::number( osmData.memberReference( index ).id() );
        writer.writeAttribute( "type", "way" );
        writer.writeAttribute( "ref", memberId );
        writer.writeAttribute( "role", "inner" );
        writer.writeEndElement();
    }
    writer.writeEndElement();
 }

void OsmRelationTagWriter::writeRelation( const QVector<GeoDataPlacemark*>& members, const QString& id,
                                          const QMap<QString,QString>& tags, GeoWriter& writer )
{
    Q_UNUSED( members );
    Q_UNUSED( id );
    Q_UNUSED( tags );
    Q_UNUSED( writer );

    // Not implemented yet ( need to implement the custom relations first )
}

}

