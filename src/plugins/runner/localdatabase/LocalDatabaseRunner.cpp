//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Torsten Rahn <rahn@kde.org>
//

#include "LocalDatabaseRunner.h"

#include "MarbleAbstractRunner.h"
#include "MarbleMap.h"
#include "MarblePlacemarkModel.h"
#include "GeoDataFeature.h"
#include "GeoDataPlacemark.h"
#include "GeoDataCoordinates.h"

#include "MarbleDebug.h"
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <QtCore/QtDebug>

namespace Marble
{

LocalDatabaseRunner::LocalDatabaseRunner(QObject *parent) : MarbleAbstractRunner(parent)
{
}

GeoDataFeature::GeoDataVisualCategory LocalDatabaseRunner::category() const
{
    return GeoDataFeature::Coordinate;
}

LocalDatabaseRunner::~LocalDatabaseRunner()
{

}

void LocalDatabaseRunner::search( const QString &searchTerm )
{
    QVector<GeoDataPlacemark*> vector;

    if (map()) {
        MarblePlacemarkModel * placemarkModel =
            qobject_cast<MarblePlacemarkModel*>( map()->placemarkModel() );

        if (placemarkModel) {
            QModelIndexList resultList;
            QModelIndex firstIndex = placemarkModel->index( 0, 0 );
            resultList = placemarkModel->approxMatch( firstIndex,
                                    Qt::DisplayRole, searchTerm, -1,
                                    Qt::MatchStartsWith );

            foreach ( const QModelIndex& index, resultList )
            {
                if( !index.isValid() ) {
                    mDebug() << "invalid index!!!";
                    continue;
                }
                QString name = index.data( Qt::DisplayRole ).toString();
                GeoDataPlacemark *placemark = new GeoDataPlacemark;
                placemark->setName( name );
                GeoDataCoordinates coordinates =
                    qvariant_cast<GeoDataCoordinates>( index.data( MarblePlacemarkModel::CoordinateRole ) );
                placemark->setCoordinate( coordinates.longitude(),
                                            coordinates.latitude() );

                int visualCategory = qvariant_cast<int>(
                    index.data( MarblePlacemarkModel::VisualCategoryRole ) );
                placemark->setVisualCategory( static_cast<GeoDataFeature::GeoDataVisualCategory>(visualCategory) );

                // FIXME: We don't always want to have it this prominent
                placemark->setPopularity( 1000000000 );
                placemark->setPopularityIndex( 18 );

                vector.append( placemark );
            }
        }
    }

    emit searchFinished( vector );
}

}

#include "LocalDatabaseRunner.moc"
