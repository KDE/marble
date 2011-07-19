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

#include "KmlColorTagHandler.h"

#include <QtGui/QColor>

#include "MarbleDebug.h"
#include "KmlElementDictionary.h"
#include "GeoDataColorStyle.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( color )

GeoNode* KmlcolorTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_color ) );

    GeoStackItem parentItem = parser.parentElement();
    
    if ( parentItem.is<GeoDataColorStyle>() ) {
        bool ok;
        QRgb abgr = parser.readElementText().trimmed().toUInt( &ok, 16 );
        unsigned a = abgr >> 24; abgr = abgr << 8; //"rgb0"
        unsigned b = abgr >> 24; abgr = abgr << 8; //"gb00"
        unsigned g = abgr >> 24; abgr = abgr << 8; //"b000"
        unsigned r = abgr >> 24;
        QRgb rgba = (a << 24)|(r << 16)|(g << 8)|(b);
        //
        // color tag uses AABBGGRR whereas QColor uses AARRGGBB - use some shifts for that
        // be aware that QRgb needs to be a typedef for 32 bit UInt for this to work properly
        if( ok ) {
            parentItem.nodeAs<GeoDataColorStyle>()->setColor( 
            QColor::fromRgba( rgba ) );
        }
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_color << "> containing: " << rgba
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }
    return 0;
}

}
}
