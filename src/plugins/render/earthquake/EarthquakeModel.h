//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydin <utkuaydin34@gmail.com>
//

#ifndef EARTHQUAKEMODEL_H
#define EARTHQUAKEMODEL_H

#include "AbstractDataPluginModel.h"

namespace Marble
{

class MarbleDataFacade;

// The maximum number of items we want to show on the screen.
const quint32 numberOfItemsOnScreen = 20;

class EarthquakeModel : public AbstractDataPluginModel
{
    Q_OBJECT

public:
    EarthquakeModel( PluginManager *pluginManager, QObject *parent = 0 );

    ~EarthquakeModel();

protected:
    /**
     * Generates the download url for the description file from the web service depending on
     * the @p box surrounding the view and the @p number of files to show.
     **/
    void getAdditionalItems( const GeoDataLatLonAltBox& box, MarbleDataFacade *facade, qint32 number = 10 );

    /**
     * Parses the @p file which getAdditionalItems downloads and
     * prepares the data for usage.
     **/
    void parseFile( const QByteArray& file );
};

}

#endif // EARTHQUAKEMODEL_H
