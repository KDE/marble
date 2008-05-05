/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2008 Simon Schmeißer <mail_to_wrt@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "GeoShapeFactory.h"

#include "GeoShape.h"
#include <KoXmlNS.h>
#include "KoShapeControllerBase.h"
#include "KoImageCollection.h"

#include <klocale.h>
#include <kdebug.h>

GeoShapeFactory::GeoShapeFactory( QObject* parent)
    : KoShapeFactory( parent, GEOSHAPEID, i18n( "Geo Shape" ) )
{
    setToolTip( i18n( "A shape which displays a map" ) );
    ///@todo setIcon( "pictureshape" );
    setIcon( "image" );
    setOdfElementNames( KoXmlNS::koffice, QStringList( "map" ) );
    setLoadingPriority( 1 );
}

KoShape* GeoShapeFactory::createDefaultShape() const
{
    return new GeoShape();
}

KoShape* GeoShapeFactory::createShape( const KoProperties* params ) const
{
    Q_UNUSED(params);
    return createDefaultShape();
}

bool GeoShapeFactory::supports(const KoXmlElement & e) const
{
    return ( e.localName() == "map" && e.namespaceURI() == KoXmlNS::koffice );
}

void GeoShapeFactory::populateDataCenterMap(QMap<QString, KoDataCenter *>   & dataCenterMap) 
{
    KoImageCollection *imgCol = new KoImageCollection();
    dataCenterMap["ImageCollection"] = imgCol;
}
