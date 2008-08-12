/*
    Copyright (C) 2008 Patrick Spendrin <ps_ml@gmx.de>

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

#include "KmlStyleTagHandler.h"

#include <QtCore/QDebug>

#include "KmlElementDictionary.h"
#include "GeoDataStyle.h"
#include "GeoDataFeature.h"
#include "GeoDataParser.h"
#include "GeoDataDocument.h"

namespace Marble
{

using namespace GeoDataElementDictionary;

KML_DEFINE_TAG_HANDLER( Style )

KmlStyleTagHandler::KmlStyleTagHandler()
    : GeoTagHandler()
{
}

KmlStyleTagHandler::~KmlStyleTagHandler()
{
}

GeoNode* KmlStyleTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_Style ) );

    GeoDataStyle* style = 0;

    GeoStackItem parentItem = parser.parentElement();
    /// for documents several styles are allowed: document wide styles are saved different!!!!!
    if( parentItem.represents( kmlTag_Document ) ) {
        style = new GeoDataStyle;
        style->setStyleId( parser.attribute( "id" ).trimmed() );
        parentItem.nodeAs<GeoDataDocument>()->addStyle( style );
#ifdef DEBUG_TAGS
    qDebug() << "Parsed <" << kmlTag_Style << "> containing: " << style << " parentItem: Document "
             << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }
    else if ( parentItem.nodeAs<GeoDataFeature>() ) {
        style = new GeoDataStyle;
        style->setStyleId( parser.attribute( "id" ).trimmed() );
        parentItem.nodeAs<GeoDataFeature>()->setStyle( style );
#ifdef DEBUG_TAGS
    qDebug() << "Parsed <" << kmlTag_Style << "> containing: " << style << " parentItem: Feature "
             << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }
    // FIXME: KMLStyle can be contained in MultiGeometry as well
    return style;
}

}
