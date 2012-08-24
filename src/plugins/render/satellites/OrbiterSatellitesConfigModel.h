//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_ORBITERSATELLITESCONFIGMODEL_H
#define MARBLE_ORBITERSATELLITESCONFIGMODEL_H

#include <QtCore/QModelIndex>

#include "SatellitesConfigModel.h"

namespace Marble {

class OrbiterSatellitesConfigModel : public SatellitesConfigModel
{

public:
    explicit OrbiterSatellitesConfigModel( QObject *parent = 0 );
};

} // namespace Marble

#endif // MARBLE_ORBITERSATELLITESCONFIGMODEL_H

