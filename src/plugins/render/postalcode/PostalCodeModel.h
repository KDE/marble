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

class PostalCodeModel : public AbstractDataPluginModel
{
    Q_OBJECT

 public:
    explicit PostalCodeModel( const MarbleModel *marbleModel, QObject *parent = nullptr );
    ~PostalCodeModel() override;

 protected:
    /**
     * Generates the download url for the description file from the web service depending on
     * the @p box surrounding the view and the @p number of files to show.
     **/
    void getAdditionalItems( const GeoDataLatLonAltBox& box,
                             qint32 number = 10 ) override;

    /**
     * Parses the @p file which getAdditionalItems downloads and
     * prepares the data for usage.
     **/
    void parseFile( const QByteArray& file ) override;

private:
    static void addLine( QString* string, const QString &line );
};

}

#endif // POSTALCODEMODEL_H
