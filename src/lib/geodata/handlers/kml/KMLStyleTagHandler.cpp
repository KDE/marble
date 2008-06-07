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

#include "KMLStyleTagHandler.h"

#include <QtCore/QDebug>

#include "KMLElementDictionary.h"
#include "GeoDataStyle.h"
#include "GeoDataFeature.h"
#include "GeoDataParser.h"

using namespace GeoDataElementDictionary;

KML_DEFINE_TAG_HANDLER( Style )

KMLStyleTagHandler::KMLStyleTagHandler()
    : GeoTagHandler()
{
}

KMLStyleTagHandler::~KMLStyleTagHandler()
{
}

GeoNode* KMLStyleTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_Style ) );

    GeoDataStyle* style = 0;
    style = new GeoDataStyle;

    GeoStackItem parentItem = parser.parentElement();
    // FIXME: it is not clear if the following is really what is needed here
    // Valid is any Feature so maybe it is Folder | NetworkLink | Document | ScreenOverlay | GroundOverlay | Placemarks
    if ( parentItem.nodeAs<GeoDataFeature>() ) {
        parentItem.nodeAs<GeoDataFeature>()->setStyle( style );
    }
    // FIXME: KMLStyle can be contained in MultiGeometry as well
#ifdef DEBUG_TAGS
    qDebug() << "Parsed <" << kmlTag_Style << "> containing: " << style
             << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    return style;
}
