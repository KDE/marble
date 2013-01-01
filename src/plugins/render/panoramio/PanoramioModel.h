//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef PANORAMIOMODEL_H
#define PANORAMIOMODEL_H

#include "AbstractDataPluginModel.h"
#include "jsonparser.h"

namespace Marble
{

const quint32 numberOfImagesPerFetch = 7;
  
class PanoramioModel : public AbstractDataPluginModel
{
    Q_OBJECT
    
 public:
    explicit PanoramioModel( QObject *parent = 0 );
 
 protected:
    /**
     * Generates the download url for the description file from the web service depending on
     * the @p box surrounding the view and the @p number of files to show.
     **/
    QUrl descriptionFileUrl( GeoDataLatLonAltBox *box, qint32 number = 10 );
       
    /**
     * The reimplementation has to parse the @p file and should generate widgets. This widgets
     * have to be scheduled to downloadWidgetData or could be directly added to the list,
     * depending on if they have to download information to be shown.
     **/
    void parseFile( QByteArray file );
    
 private:
    jsonParser panoramioJsonParser;
};

}

#endif // PANORAMIOMODEL_H
