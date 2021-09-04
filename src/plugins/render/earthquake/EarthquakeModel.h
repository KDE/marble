// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Utku Aydin <utkuaydin34@gmail.com>
//

#ifndef EARTHQUAKEMODEL_H
#define EARTHQUAKEMODEL_H

#include "AbstractDataPluginModel.h"
#include <QDateTime>

namespace Marble
{

class EarthquakeModel : public AbstractDataPluginModel
{
    Q_OBJECT

public:
    explicit EarthquakeModel( const MarbleModel *marbleModel, QObject *parent = nullptr );
    ~EarthquakeModel() override;

    void setMinMagnitude( double minMagnitude );
    void setStartDate( const QDateTime& startDate );
    void setEndDate( const QDateTime& endDate );

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
    double m_minMagnitude;
    QDateTime m_startDate;
    QDateTime m_endDate;
};

}

#endif // EARTHQUAKEMODEL_H
