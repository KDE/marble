//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Valery Kharitonov <kharvd@gmail.com>
//

#ifndef POSTALCODEMODEL_H
#define POSTALCODEMODEL_H

#include "AbstractDataPluginModel.h"

namespace Marble {

class MarbleModel;

// The maximum number of items we want to show on the screen.
const quint32 numberOfItemsOnScreen = 20;

class PostalCodeModel : public AbstractDataPluginModel
{
    Q_OBJECT

 public:
    PostalCodeModel( const PluginManager *pluginManager,
                   QObject *parent = 0 );
    ~PostalCodeModel();

 protected:
    /**
     * Generates the download url for the description file from the web service depending on
     * the @p box surrounding the view and the @p number of files to show.
     **/
    void getAdditionalItems( const GeoDataLatLonAltBox& box,
                             const MarbleModel *model,
                             qint32 number = 10 );

    /**
     * Parses the @p file which getAdditionalItems downloads and
     * prepares the data for usage.
     **/
    void parseFile( const QByteArray& file );

private:
    void addLine( QString* string, const QString &line );
};

}

#endif // POSTALCODEMODEL_H
