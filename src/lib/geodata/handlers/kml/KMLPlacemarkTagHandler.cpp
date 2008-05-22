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

#include "KMLPlacemarkTagHandler.h"

#include <QtCore/QDebug>

#include "KMLElementDictionary.h"
#include "GeoDataContainer.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"

using namespace GeoDataElementDictionary;

KML_DEFINE_TAG_HANDLER( Placemark )

KMLPlacemarkTagHandler::KMLPlacemarkTagHandler()
    : GeoTagHandler()
{
}

KMLPlacemarkTagHandler::~KMLPlacemarkTagHandler()
{
}

GeoNode* KMLPlacemarkTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_Placemark ) );

    GeoDataPlacemark* placemark = 0;
    placemark = new GeoDataPlacemark;

    GeoStackItem parentItem = parser.parentElement();
    if( parentItem.represents( kmlTag_Folder ) || parentItem.represents( kmlTag_Document ) ) {
        parentItem.nodeAs<GeoDataContainer>()->addFeature( placemark );
    }
    qDebug() << "Parsed <Placemark> start! Created GeoDataPlacemark item: " << placemark 
             << " parent item name: " << parentItem.qualifiedName().first 
             << " associated item: " << parentItem.associatedNode();

    return placemark;
}
