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

#include <QColor>

#include "MarbleDebug.h"
#include "KmlElementDictionary.h"
#include "GeoDataColorStyle.h"
#include "GeoDataOverlay.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( color )

GeoNode* KmlcolorTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_color)));

    GeoStackItem parentItem = parser.parentElement();
    
    if ( parentItem.is<GeoDataColorStyle>() || parentItem.is<GeoDataOverlay>() ) {
        QColor const color = parseColor( parser.readElementText().trimmed() );
        if ( parentItem.is<GeoDataColorStyle>() ) {
            parentItem.nodeAs<GeoDataColorStyle>()->setColor( color );
        } else if ( parentItem.is<GeoDataOverlay>() ) {
            parentItem.nodeAs<GeoDataOverlay>()->setColor( color );
        }
    }
    return 0;
}

QColor KmlcolorTagHandler::parseColor( const QString &colorString )
{
    // color tag uses AABBGGRR whereas QColor uses AARRGGBB - use some shifts for that
    // be aware that QRgb needs to be a typedef for 32 bit UInt for this to work properly
    bool ok;
    QRgb abgr = colorString.toUInt( &ok, 16 );
    unsigned a = abgr >> 24; abgr = abgr << 8; //"rgb0"
    unsigned b = abgr >> 24; abgr = abgr << 8; //"gb00"
    unsigned g = abgr >> 24; abgr = abgr << 8; //"b000"
    unsigned r = abgr >> 24;
    QRgb rgba = (a << 24)|(r << 16)|(g << 8)|(b);
    return ok ? QColor::fromRgba( rgba ) : QColor();
}

}
}
