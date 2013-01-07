//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlListStyleTagWriter.h"

#include "GeoDataListStyle.h"
#include "GeoDataItemIcon.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlColorStyleTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerListStyle( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataListStyleType,
                                                                              kml::kmlTag_nameSpace22),
                                                  new KmlListStyleTagWriter() );

bool KmlListStyleTagWriter::write( const GeoNode *node,
                                   GeoWriter& writer ) const
{
    const GeoDataListStyle *listStyle = static_cast<const GeoDataListStyle*>( node );
    writer.writeStartElement( kml::kmlTag_ListStyle );

    QString listItemTypeString;
    switch (listStyle->listItemType())
    {
    case GeoDataListStyle::CheckOffOnly:
        listItemTypeString = "checkOffOnly";
        break;

    case GeoDataListStyle::CheckHideChildren:
        listItemTypeString = "checkHideChildren";
        break;

    case GeoDataListStyle::RadioFolder:
        listItemTypeString = "radioFolder";
        break;

    default:
        listItemTypeString = "check";
    }
    writer.writeStartElement(kml::kmlTag_listItemType);
    writer.writeCharacters(listItemTypeString);
    writer.writeEndElement();

    writer.writeStartElement(kml::kmlTag_bgColor);
    writer.writeCharacters( KmlColorStyleTagWriter::formatColor( listStyle->backgroundColor() ) );
    writer.writeEndElement();

    foreach( GeoDataItemIcon* icon, listStyle->itemIconList() )
    {
        writer.writeStartElement(kml::kmlTag_ItemIcon);

        QStringList stateList;
        if ( icon->state() & GeoDataItemIcon::Open ) {
            stateList << "open";
        }
        if ( icon->state() & GeoDataItemIcon::Closed ) {
            stateList << "closed";
        }
        if ( icon->state() & GeoDataItemIcon::Error ) {
            stateList << "error";
        }
        if ( icon->state() & GeoDataItemIcon::Fetching0 ) {
            stateList << "fetching0";
        }
        if ( icon->state() & GeoDataItemIcon::Fetching1 ) {
            stateList << "fetching1";
        }
        if ( icon->state() & GeoDataItemIcon::Fetching2 ) {
            stateList << "fetching2";
        }
        QString const states = stateList.join(" ");
        writer.writeStartElement(kml::kmlTag_state);
        writer.writeCharacters( states );
        writer.writeEndElement();

        writer.writeStartElement(kml::kmlTag_href);
        writer.writeCharacters( icon->iconPath() );
        writer.writeEndElement();

        writer.writeEndElement();
    }

    writer.writeEndElement();
    return true;
}

}
