//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "EarthSatellitesConfigModel.h"

#include "SatellitesConfigNodeItem.h"
#include "SatellitesConfigLeafItem.h"
#include "MarbleDebug.h"

namespace Marble {

EarthSatellitesConfigModel::EarthSatellitesConfigModel( QObject *parent )
    : SatellitesConfigModel( parent )
{
}

QStringList EarthSatellitesConfigModel::tleList()
{
    return m_rootItem->data( 0, SatellitesConfigAbstractItem::UrlListRole ).toStringList();
}

} // namespace Marble

