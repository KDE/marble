//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "OrbiterSatellitesConfigModel.h"

#include "SatellitesConfigNodeItem.h"
#include "SatellitesConfigLeafItem.h"
#include "MarbleDebug.h"

namespace Marble {

OrbiterSatellitesConfigModel::OrbiterSatellitesConfigModel( QObject *parent )
    : SatellitesConfigModel( parent )
{
}

} // namespace Marble

