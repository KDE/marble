//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef BBCWEATHERSERVICE_H
#define BBCWEATHERSERVICE_H

#include "AbstractWeatherService.h"

namespace Marble {

class BBCWeatherService : public AbstractWeatherService {
    Q_OBJECT
 
 public:
    BBCWeatherService( QObject *parent );
    ~BBCWeatherService();
    
 public Q_SLOTS:
    void getAdditionalItems( const GeoDataLatLonAltBox& box,
                             MarbleDataFacade *facade,
                             qint32 number = 10 );
};

} // namespace Marble

#endif // BBCWEATHERSERVICE_H
