//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mayank Madan <maddiemadan@gmail.com>
// Copyright 2013      Dennis Nienhüser <earthwings@gentoo.org>
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

    QString const itemType = itemTypeToString( listStyle->listItemType() );
    writer.writeOptionalElement( kml::kmlTag_listItemType, itemType, "check" );
    QString const color = KmlColorStyleTagWriter::formatColor( listStyle->backgroundColor() );
    writer.writeOptionalElement( kml::kmlTag_bgColor, color, "ffffffff" );

    foreach( GeoDataItemIcon* icon, listStyle->itemIconList() ) {
        writer.writeStartElement(kml::kmlTag_ItemIcon);
        QString const state = iconStateToString( icon->state() );
        writer.writeOptionalElement( kml::kmlTag_state, state, "open" );
        writer.writeOptionalElement( kml::kmlTag_href, icon->iconPath() );
        writer.writeEndElement();
    }

    writer.writeEndElement();
    return true;
}

QString KmlListStyleTagWriter::itemTypeToString( GeoDataListStyle::ListItemType itemType ) const
{
    switch ( itemType )
    {
    case GeoDataListStyle::CheckOffOnly:      return "checkOffOnly";
    case GeoDataListStyle::CheckHideChildren: return "checkHideChildren";
    case GeoDataListStyle::RadioFolder:       return "radioFolder";
    default:                                  return "check";
    }
}

QString KmlListStyleTagWriter::iconStateToString( GeoDataItemIcon::ItemIconStates state ) const
{
    QStringList stateList;
    if ( state & GeoDataItemIcon::Open ) {
        stateList << "open";
    }
    if ( state & GeoDataItemIcon::Closed ) {
        stateList << "closed";
    }
    if ( state & GeoDataItemIcon::Error ) {
        stateList << "error";
    }
    if ( state & GeoDataItemIcon::Fetching0 ) {
        stateList << "fetching0";
    }
    if ( state & GeoDataItemIcon::Fetching1 ) {
        stateList << "fetching1";
    }
    if ( state & GeoDataItemIcon::Fetching2 ) {
        stateList << "fetching2";
    }

    return stateList.join(" ");
}

}
