// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Valery Kharitonov <kharvd@gmail.com>
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
