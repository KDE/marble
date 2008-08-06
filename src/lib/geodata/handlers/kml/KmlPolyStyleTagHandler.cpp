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

#include "KmlPolyStyleTagHandler.h"

#include <QtCore/QDebug>

#include "KmlElementDictionary.h"
#include "GeoDataStyle.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataParser.h"

using namespace GeoDataElementDictionary;

KML_DEFINE_TAG_HANDLER( PolyStyle )

KmlPolyStyleTagHandler::KmlPolyStyleTagHandler()
    : GeoTagHandler()
{
}

KmlPolyStyleTagHandler::~KmlPolyStyleTagHandler()
{
}

GeoNode* KmlPolyStyleTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_PolyStyle ) );

    GeoStackItem parentItem = parser.parentElement();
    
    GeoDataPolyStyle* style = 0;
    
    if ( parentItem.represents( kmlTag_Style ) ) {
        style = new GeoDataPolyStyle();
        parentItem.nodeAs<GeoDataStyle>()->setPolyStyle( style );
#ifdef DEBUG_TAGS
        qDebug() << "Parsed <" << kmlTag_PolyStyle << "> containing: " << style
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }
    return style;
}
