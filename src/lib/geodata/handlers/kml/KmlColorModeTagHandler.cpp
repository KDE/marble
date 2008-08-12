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

#include "KmlColorModeTagHandler.h"

#include <QtCore/QDebug>
#include <QtGui/QColor>

#include "KmlElementDictionary.h"
#include "GeoDataColorStyle.h"
#include "GeoDataFeature.h"
#include "GeoDataParser.h"

namespace Marble
{

using namespace GeoDataElementDictionary;

KML_DEFINE_TAG_HANDLER( colorMode )

KmlcolorModeTagHandler::KmlcolorModeTagHandler()
    : GeoTagHandler()
{
}

KmlcolorModeTagHandler::~KmlcolorModeTagHandler()
{
}

GeoNode* KmlcolorModeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_colorMode ) );

    GeoStackItem  parentItem = parser.parentElement();
    
    if ( parentItem.represents( kmlTag_ColorStyle ) ) {
        if ( parser.readElementText().trimmed() == QString("random") ) 
	    parentItem.nodeAs<GeoDataColorStyle>()->setColorMode( GeoDataColorStyle::Random );
        else
	    parentItem.nodeAs<GeoDataColorStyle>()->setColorMode( GeoDataColorStyle::Normal );
#ifdef DEBUG_TAGS
        qDebug() << "Parsed <" << kmlTag_ColorStyle << "> containing: " << parser.readElementText().trimmed()
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
    }

    return 0;
}

}
