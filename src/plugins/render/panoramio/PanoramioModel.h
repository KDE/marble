// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef PANORAMIOMODEL_H
#define PANORAMIOMODEL_H

#include "AbstractDataPluginModel.h"

namespace Marble
{

class MarbleWidget;

const quint32 numberOfImagesPerFetch = 7;

class PanoramioModel : public AbstractDataPluginModel
{
    Q_OBJECT

 public:
    explicit PanoramioModel( const MarbleModel *marbleModel, QObject *parent = 0 );

    void setMarbleWidget( MarbleWidget *widget );

 protected:
    /**
     * Generates the download url for the description file from the web service depending on
     * the @p box surrounding the view and the @p number of files to show.
     **/
    void getAdditionalItems( const GeoDataLatLonAltBox &box, qint32 number = 10 );

    /**
     * The reimplementation has to parse the @p file and should generate widgets. This widgets
     * have to be scheduled to downloadWidgetData or could be directly added to the list,
     * depending on if they have to download information to be shown.
     **/
    void parseFile( const QByteArray &file );

private:
    MarbleWidget *m_marbleWidget;
};

}

#endif // PANORAMIOMODEL_H
