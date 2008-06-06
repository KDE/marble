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

#if KML_LAZY_IMP
#include "KMLCountrycodeTagHandler.h"

#include <QtCore/QDebug>

#include "KMLElementDictionary.h"

#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"

using namespace GeoDataElementDictionary;

KML_DEFINE_TAG_HANDLER( countrycode )

KMLcountrycodeTagHandler::KMLcountrycodeTagHandler()
    : GeoTagHandler()
{
}

KMLcountrycodeTagHandler::~KMLcountrycodeTagHandler()
{
}

GeoNode* KMLcountrycodeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_countrycode ) );

    GeoStackItem parentItem = parser.parentElement();
    
    if( parentItem.nodeAs<GeoDataPlacemark>() ) {
        QString countrycode = parser.readElementText().trimmed();
        
        parentItem.nodeAs<GeoDataPlacemark>()->setCountryCode( countrycode );
        qDebug() << "Parsed <" << kmlTag_countrycode << "> containing: " << countrycode
                 << " parent item name: " << parentItem.qualifiedName().first;
    }

    return 0;
}
#endif //KML_LAZY_IMP
