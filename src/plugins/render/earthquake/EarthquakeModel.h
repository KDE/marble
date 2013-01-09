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
#include <QtCore/QDateTime>

namespace Marble
{

class MarbleModel;

class EarthquakeModel : public AbstractDataPluginModel
{
    Q_OBJECT

public:
    explicit EarthquakeModel( QObject *parent = 0 );
    ~EarthquakeModel();

    void setMinMagnitude( double minMagnitude );
    void setStartDate( const QDateTime& startDate );
    void setEndDate( const QDateTime& endDate );

protected:
    /**
     * Generates the download url for the description file from the web service depending on
     * the @p box surrounding the view and the @p number of files to show.
     **/
    virtual void getAdditionalItems( const GeoDataLatLonAltBox& box,
                                     const MarbleModel *model,
                                     qint32 number = 10 );

    /**
     * Parses the @p file which getAdditionalItems downloads and
     * prepares the data for usage.
     **/
    void parseFile( const QByteArray& file );

private:
    double m_minMagnitude;
    QDateTime m_startDate;
    QDateTime m_endDate;
};

}

#endif // EARTHQUAKEMODEL_H
