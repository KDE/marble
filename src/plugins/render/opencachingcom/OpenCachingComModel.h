//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Anders Lund <anders@alweb.dk>
//

#ifndef OPENCACHINGCOMMODEL_H
#define OPENCACHINGCOMMODEL_H

#include "AbstractDataPluginModel.h"
#include "GeoDataLatLonAltBox.h"

#include <QtCore/QDateTime>

// Please note and respect that this key is for use ONLY within this plugin.
#define AUTHKEY "mJg2Q5fD3qczP7M8"

namespace Marble
{

class MarbleModel;

class OpenCachingComItem;

namespace {
// The maximum number of items we want to show on the screen.
const quint32 numberOfItemsOnScreen = 100;
}

class OpenCachingComModel : public AbstractDataPluginModel
{
    Q_OBJECT

public:
    explicit OpenCachingComModel( const MarbleModel *marbleModel, QObject *parent = 0 );
    ~OpenCachingComModel();


    /// Fetch a file for a cache
    void fetchData(const QString& url, const QString &type, OpenCachingComItem *item);

    /// access MarbleModel
    const MarbleModel* marbleModel();

    /// home location
    const GeoDataCoordinates home() const;

protected:
    /**
     * Generates the download url for the description file from the web service depending on
     * the @p box surrounding the view and the @p number of files to show.
     **/
    virtual void getAdditionalItems( const GeoDataLatLonAltBox& box,
                                     const MarbleModel *model,
                                     qint32 number = numberOfItemsOnScreen );

    /**
     * Parses the @p file which getAdditionalItems downloads and
     * prepares the data for usage.
     **/
    void parseFile( const QByteArray& file );

private slots:
    void updateHome();

private:
    GeoDataLatLonAltBox m_previousbox;
    GeoDataCoordinates m_homeCoordinates;
    const MarbleModel *m_marbleModel;
};

}

#endif // OPENCACHINGCOMMODEL_H
