//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef OPENDESKTOPMODEL_H
#define OPENDESKTOPMODEL_H
 
#include "AbstractDataPluginModel.h"
 
namespace Marble {
 
class MarbleWidget;
class MarbleModel;
 
class OpenDesktopModel : public AbstractDataPluginModel
{
    Q_OBJECT
 
public:
    explicit OpenDesktopModel( const MarbleModel *marbleModel, QObject *parent = 0 );
    ~OpenDesktopModel() override;

    void setMarbleWidget(MarbleWidget *widget);

protected:
    /**
     * Generates the download url for the description file from the web service depending on
     * the @p box surrounding the view and the @p number of files to show.
     **/
    void getAdditionalItems(const Marble::GeoDataLatLonAltBox& box, qint32 number = 10) override;
    /**
     * Parses the @p file which getAdditionalItems downloads and
     * prepares the data for usage.
     **/
    void parseFile(const QByteArray& file) override;

private:
    MarbleWidget *m_marbleWidget;
};
 
}
 
#endif // OPENDESKTOPMODEL_H
