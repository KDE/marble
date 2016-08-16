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

#include "KmlHeadingTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataIconStyle.h"
#include "GeoDataCamera.h"
#include "GeoDataOrientation.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( heading )

GeoNode* KmlheadingTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_heading)));

    GeoStackItem parentItem = parser.parentElement();
    
    int const heading = parser.readElementText().toInt();
    if ( heading >= 0 && heading <= 360 ) {
        if ( parentItem.represents( kmlTag_IconStyle ) ) {
            parentItem.nodeAs<GeoDataIconStyle>()->setHeading( heading );
        } else if ( parentItem.represents( kmlTag_Camera ) ) {
            parentItem.nodeAs<GeoDataCamera>()->setHeading( heading );
        } else if ( parentItem.represents( kmlTag_Orientation ) ) {
	    parentItem.nodeAs<GeoDataOrientation>()->setHeading( heading );
	}
    } else {
        mDebug() << "Invalid heading value " << heading << ", must be within 0..360. Using 0 instead.";
    }

    return 0;
}

}
}
