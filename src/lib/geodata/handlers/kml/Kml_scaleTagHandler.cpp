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

// be aware that there is another Tag called Scale which uses the name KmlscaleTagHandler.h
// as it is impossible to use the name KmlscaleTagHandler then, use an underscore
// to mark the lower case variant

#include "Kml_scaleTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( scale )

GeoNode* KmlscaleTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_scale ) );

    GeoStackItem parentItem = parser.parentElement();
    
    if ( parentItem.represents( kmlTag_IconStyle ) ) {
        parentItem.nodeAs<GeoDataIconStyle>()->setScale( 
        parser.readElementText().trimmed().toFloat() );
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_scale << "> containing: " << parser.readElementText().trimmed()
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    } else if( parentItem.represents( kmlTag_LabelStyle ) ) {
        parentItem.nodeAs<GeoDataLabelStyle>()->setScale( 
        parser.readElementText().trimmed().toFloat() );
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_scale << "> containing: " << parser.readElementText().trimmed()
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }
    return 0;
}

}
}
