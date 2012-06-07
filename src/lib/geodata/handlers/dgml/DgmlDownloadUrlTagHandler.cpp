/*
    Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

// Own
#include "DgmlDownloadUrlTagHandler.h"

// Qt
#include <QtCore/QUrl>

// Marble
#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTiled.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(DownloadUrl)

GeoNode* DgmlDownloadUrlTagHandler::parse( GeoParser& parser ) const
{
    // Check whether the tag is valid
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( dgmlTag_DownloadUrl ));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if ( !parentItem.represents( dgmlTag_Texture ) &&  !parentItem.represents( dgmlTag_VectorTile ) )
        return 0;

    QUrl url;

    // Attribute protocol, optional?
    QString protocolStr = parser.attribute( dgmlAttr_protocol ).trimmed();
    if ( !protocolStr.isEmpty() )
        url.setScheme( protocolStr );

    // Attribute user, optional
    const QString userStr = parser.attribute( dgmlAttr_user ).trimmed();
    if ( !userStr.isEmpty() )
        url.setUserName( userStr );

    // Attribute password, optional
    const QString passwordStr = parser.attribute( dgmlAttr_password ).trimmed();
    if ( !passwordStr.isEmpty() )
        url.setPassword( passwordStr );

    // Attribute host, mandatory
    const QString hostStr = parser.attribute( dgmlAttr_host ).trimmed();
    if ( !hostStr.isEmpty() )
        url.setHost( hostStr );

    // Attribute port, optional
    const QString portStr = parser.attribute( dgmlAttr_port ).trimmed();
    if ( !portStr.isEmpty() )
        url.setPort( portStr.toInt() );

    // Attribute path, mandatory
    const QString pathStr = parser.attribute( dgmlAttr_path ).trimmed();
    if ( !pathStr.isEmpty() )
        url.setPath( pathStr );

    // Attribute query, optional
    const QString queryStr = parser.attribute( dgmlAttr_query ).trimmed();
    if ( !queryStr.isEmpty() )
        url.setEncodedQuery( queryStr.toLatin1() );

    parentItem.nodeAs<GeoSceneTiled>()->addDownloadUrl( url );
    return 0;
}

}
}
