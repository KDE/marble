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

#include <QtCore/QDebug>
#include <QtGui/QColor>

#include "KmlElementDictionary.h"
#include "GeoDataColorStyle.h"
#include "GeoDataParser.h"

using namespace GeoDataElementDictionary;

KML_DEFINE_TAG_HANDLER( color )

KmlcolorTagHandler::KmlcolorTagHandler()
    : GeoTagHandler()
{
}

KmlcolorTagHandler::~KmlcolorTagHandler()
{
}

GeoNode* KmlcolorTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_color ) );

    GeoStackItem parentItem = parser.parentElement();
    
    if ( parentItem.nodeAs<GeoDataColorStyle>() ) {
        bool ok;
        QRgb rgba = parser.readElementText().trimmed().toUInt( &ok, 16 );
        // color tag uses RRGGBBAA whereas QColor uses AARRGGBB - use a circular shift for that
        // be aware that QRgb needs to be a typedef for 32 bit UInt for this to work properly
        //        rgba = ( rgba << 24 ) | ( rgba >> 8 );
        // after checking against GE it seems as if this isn't needed
        if( ok ) {
            parentItem.nodeAs<GeoDataColorStyle>()->setColor( 
            QColor::fromRgba( rgba ) );
        }
#ifdef DEBUG_TAGS
        qDebug() << "Parsed <" << kmlTag_color << "> containing: " << rgba
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }
    return 0;
}
