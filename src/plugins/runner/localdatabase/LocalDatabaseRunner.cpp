//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Torsten Rahn <rahn@kde.org>
//

#include "LocalDatabaseRunner.h"

#include "MarbleAbstractRunner.h"
#include "MarbleModel.h"
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

void LocalDatabaseRunner::search( const QString &searchTerm, const GeoDataLatLonAltBox preferred )
{
    QVector<GeoDataPlacemark*> vector;

    if (model()) {
        QAbstractItemModel * placemarkModel = model()->placemarkModel();

        if (placemarkModel) {
            QModelIndexList resultList;
            QModelIndex firstIndex = placemarkModel->index( 0, 0 );
            resultList = placemarkModel->match( firstIndex,
                                    Qt::DisplayRole, searchTerm, -1,
                                    Qt::MatchStartsWith );

            foreach ( const QModelIndex& index, resultList )
            {
                if( !index.isValid() ) {
                    mDebug() << "invalid index!!!";
                    continue;
                }
                GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>(qvariant_cast<GeoDataObject*>( index.data( MarblePlacemarkModel::ObjectPointerRole )));
                if ( placemark &&
                     ( preferred.isEmpty() || preferred.contains( placemark->coordinate() ) ) ) {
                    vector.append( new GeoDataPlacemark( *placemark ));
                }
            }
        }
    }

    emit searchFinished( vector );
}

}

#include "LocalDatabaseRunner.moc"
