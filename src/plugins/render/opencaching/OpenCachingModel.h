//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#ifndef OPENCACHINGMODEL_H
#define OPENCACHINGMODEL_H

#include "AbstractDataPluginModel.h"
#include <QtCore/QDateTime>

namespace Marble
{

class MarbleModel;

class OpenCachingModelPrivate;

namespace {
// The maximum number of items we want to show on the screen.
const quint32 numberOfItemsOnScreen = 20;
}

/**
 * Model that manages cache information.
 */
class OpenCachingModel : public AbstractDataPluginModel
{
    Q_OBJECT

public:
    explicit OpenCachingModel( PluginManager *pluginManager, QObject *parent = 0 );
    ~OpenCachingModel();

    void setNumResults( int numResults );
    void setMaxDistance( int maxDistance );
    void setMinDifficulty( double minDifficulty );
    void setMaxDifficulty( double maxDifficulty );
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
    int m_numResults;
    int m_maxDistance;
    qreal m_minDifficulty;
    qreal m_maxDifficulty;
    QDateTime m_startDate;
    QDateTime m_endDate;
    OpenCachingModelPrivate *const d;
};

}

#endif // OPENCACHINGMODEL_H
