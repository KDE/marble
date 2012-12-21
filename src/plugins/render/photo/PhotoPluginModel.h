//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef PHOTOPLUGINMODEL_H
#define PHOTOPLUGINMODEL_H

#include "AbstractDataPluginModel.h"

namespace Marble
{

class MarbleModel;
class MarbleWidget;

const quint32 numberOfImagesPerFetch = 15;
  
class PhotoPluginModel : public AbstractDataPluginModel
{
    Q_OBJECT
    
 public:
    explicit PhotoPluginModel( const PluginManager *pluginManager, QObject *parent = 0 );
    
    static QUrl generateUrl( const QString& service,
                             const QString& method,
                             const QHash<QString,QString>& options );

    void setMarbleWidget( MarbleWidget* widget );
 
 protected:
    /**
     * Generates the download url for the description file from the web service depending on
     * the @p box surrounding the view and the @p number of files to show.
     **/
    void getAdditionalItems( const GeoDataLatLonAltBox& box,
                             const MarbleModel *model,
                             qint32 number = 10 );
       
    /**
     * The reimplementation has to parse the @p file and should generate items. This items
     * have to be scheduled to downloadItemData or could be directly added to the list,
     * depending on if they have to download information to be shown.
     **/
    void parseFile( const QByteArray& file );

 private:
    MarbleWidget *m_marbleWidget;
};

}

#endif //PHOTOPLUGINMODEL_H
