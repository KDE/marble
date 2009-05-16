//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


// Own
#include "MarblePlacemarkModel.h"
#include "MarblePlacemarkModel_P.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QModelIndex>
#include <QtCore/QTime>
#include <QtGui/QPixmap>

// Marble
#include "GeoDataStyle.h"       // In geodata/data/
#include "PlacemarkContainer.h"
#include "PlacemarkManager.h"

using namespace Marble;

class MarblePlacemarkModel::Private
{
    friend class PlacemarkManager;

 public:
    Private( PlacemarkManager *manager, MarblePlacemarkModel *parent )
        : m_parent( parent ), m_manager( manager )
    {
    }

    ~Private()
    {
    }

    MarblePlacemarkModel  *m_parent;
    PlacemarkManager      *m_manager;
    PlacemarkContainer     m_placemarkContainer;
};


// ---------------------------------------------------------------------------


MarblePlacemarkModel::MarblePlacemarkModel( PlacemarkManager *manager, 
                                            QObject *parent )
    : QAbstractListModel( parent ),
      d( new Private( manager, this ) )
{
    // Register at PlacemarkManager
    if ( d->m_manager )
        d->m_manager->setPlacemarkModel( this );
}

MarblePlacemarkModel::~MarblePlacemarkModel()
{
    clearPlacemarks();
    // Unregister from PlacemarkManager
    if ( d->m_manager )
        d->m_manager->setPlacemarkModel( 0 );

    delete d;
}

void MarblePlacemarkModel::sort( int column, Qt::SortOrder order )
{
    Q_UNUSED( column )
    Q_UNUSED( order )

    QTime t;
    t.start();
    qDebug() << "start sorting";

    emit layoutAboutToBeChanged();
//    d->m_placemarkContainer.sort( order );
    emit layoutChanged();

    qDebug() << "MarblePlacemarkModel (sort): Time elapsed:" << t.elapsed() << "ms";
}

int MarblePlacemarkModel::rowCount( const QModelIndex &parent ) const
{
    if ( !parent.isValid() )
        return d->m_placemarkContainer.size();
    else
        return 0;
}

int MarblePlacemarkModel::columnCount( const QModelIndex &parent ) const
{
    if ( !parent.isValid() )
        return 1;
    else
        return 0;
}

GeoDataCoordinates MarblePlacemarkModel::coordinateData( const QModelIndex &index ) const
{
    if ( !index.isValid() ) {
        qDebug() << "MarblePlacemarkModel: Error - index invalid";
        return GeoDataCoordinates();
    }
    return d->m_placemarkContainer.at( index.row() ).coordinate();
}

GeoDataStyle* MarblePlacemarkModel::styleData( const QModelIndex &index ) const
{
    if ( !index.isValid() )
        return 0;
    else {
        return d->m_placemarkContainer.at( index.row() ).style();
    }
}

QVariant MarblePlacemarkModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( index.row() >= d->m_placemarkContainer.size() )
        return QVariant();

    if ( role == Qt::DisplayRole ) {
        return d->m_placemarkContainer.at( index.row() ).name();
    } else if ( role == Qt::DecorationRole ) {
          return qVariantFromValue( d->m_placemarkContainer.at( index.row() ).style()->iconStyle().icon() );
    } else if ( role == PopularityIndexRole ) {
        return d->m_placemarkContainer.at( index.row() ).popularityIndex();
    } else if ( role == VisualCategoryRole ) {
        return d->m_placemarkContainer.at( index.row() ).visualCategory();
    } else if ( role == AreaRole ) {
        return d->m_placemarkContainer.at( index.row() ).area();
    } else if ( role == PopulationRole ) {
        return d->m_placemarkContainer.at( index.row() ).population();
    } else if ( role == CountryCodeRole ) {
        return d->m_placemarkContainer.at( index.row() ).countryCode();
    } else if ( role == PopularityRole ) {
        return d->m_placemarkContainer.at( index.row() ).popularity();
    } else if ( role == DescriptionRole ) {
        return d->m_placemarkContainer.at( index.row() ).description();
    } else if ( role == Qt::ToolTipRole ) {
        return d->m_placemarkContainer.at( index.row() ).description();
    } else if ( role == GeoTypeRole ) {
        return d->m_placemarkContainer.at( index.row() ).role();
    } else if ( role == CoordinateRole ) {
        return qVariantFromValue( d->m_placemarkContainer.at( index.row() ).coordinate() );
    } else if ( role == StyleRole ) {
        return qVariantFromValue( d->m_placemarkContainer.at( index.row() ).style() );
/*    } else if ( role == ObjectPointerRole ) {
        return qVariantFromValue( dynamic_cast<GeoDataObject*>( d->m_placemarkContainer.at( index.row() ) ) );*/
    } else
        return QVariant();
}

QModelIndexList MarblePlacemarkModel::approxMatch( const QModelIndex & start, int role, 
                                             const QVariant & value, int hits,
                                             Qt::MatchFlags flags ) const
{
    QList<QModelIndex> results;

    int         count = 0;

    QModelIndex entryIndex;
    QString     listName;
    QString     queryString = value.toString().toLower();
    QString     simplifiedListName;

    int         row = start.row();
    const int   rowNum = rowCount();

    while ( row < rowNum && count != hits ) {
        if ( flags & Qt::MatchStartsWith ) {
            entryIndex = index( row, 0 );
            listName    = data( entryIndex, role ).toString().toLower();
            simplifiedListName = GeoString::deaccent( listName );

            if ( listName.startsWith( queryString ) 
                 || simplifiedListName.startsWith( queryString )
                 )
            {
                results << entryIndex;
                ++count;
            }
        }
        ++row;
    }

    return results;
}

void MarblePlacemarkModel::addPlacemarks( PlacemarkContainer &placemarks, 
                                          bool clearPrevious,
                                          bool finalize )
{
    Q_UNUSED( finalize )

    if( placemarks.count() <= 0 )
        return;

    if ( clearPrevious ) {
        beginRemoveRows( QModelIndex(), 0, rowCount() );

        d->m_placemarkContainer.clear();

        endRemoveRows();
    }

    beginInsertRows( QModelIndex(), 0, placemarks.count() - 1 );
    createFilterProperties( placemarks );
    d->m_placemarkContainer << placemarks;

    endInsertRows();

    emit dataChanged( index( 0, 0 ), index( placemarks.count() - 1, 0 ) );
}

void  MarblePlacemarkModel::removePlacemarks( const QString &containerName,
                                              int start,
                                              int length,
                                              bool finalize )
{
    Q_UNUSED( finalize )

    QTime t;
    t.start();

    beginRemoveRows( QModelIndex(), 0, length );
    QVector<Marble::GeoDataPlacemark>::iterator begin = d->m_placemarkContainer.begin() + start;
    QVector<Marble::GeoDataPlacemark>::iterator end = d->m_placemarkContainer.begin() + start + length;
    d->m_placemarkContainer.erase(begin, end);
    endRemoveRows();

/*    if ( finalize ) {
//          sort( Qt::DescendingOrder );
    }*/
    // there have not been any additions, but without the following line marble seems to crash here.
    emit dataChanged( index( 0, 0 ), index( 0, 0 ) );
    qDebug() << "removePlacemarks(" << containerName << "): Time elapsed:" << t.elapsed() << "ms for" << length << "Placemarks.";
}

QStringList MarblePlacemarkModel::containers() const
{
    return QStringList();
}

void MarblePlacemarkModel::clearPlacemarks()
{
    d->m_placemarkContainer.clear();
    reset();
}

void MarblePlacemarkModel::createFilterProperties( PlacemarkContainer &container )
{

    QVector<GeoDataPlacemark>::Iterator i;
    for ( i = container.begin(); i != container.end(); ++i ) {
        GeoDataPlacemark placemark = *i;

        bool hasPopularity = false;

        // Mountain (H), Volcano (V), Shipwreck (W)
        if ( placemark.role() == 'H' || placemark.role() == 'V' || placemark.role() == 'W' )
        {
            qreal altitude = placemark.coordinate().altitude();
            if ( altitude != 0.0 )
            {
                hasPopularity = true;
                placemark.setPopularity( (qint64)(altitude * 1000.0) );
                placemark.setPopularityIndex( cityPopIdx( qAbs( (qint64)(altitude * 1000.0) ) ) );
            }
        }
        // Continent (K), Ocean (O), Nation (S)
        else if ( placemark.role() == 'K' || placemark.role() == 'O' || placemark.role() == 'S' )
        {
            qreal area = placemark.area();
            if ( area >= 0.0 )
            {
                hasPopularity = true;
//                qDebug() << placemark->name() << " " << (qint64)(area);
                placemark.setPopularity( (qint64)(area * 100) );
                placemark.setPopularityIndex( areaPopIdx( area ) );
            }
        }
        // Pole (P)
        else if ( placemark.role() == 'P' )
        {
            placemark.setPopularity( 1000000000 );
            placemark.setPopularityIndex( 18 );
        }
        // Magnetic Pole (M)
        else if ( placemark.role() == 'M' )
        {
            placemark.setPopularity( 10000000 );
            placemark.setPopularityIndex( 13 );
        }
        // MannedLandingSite (h)
        else if ( placemark.role() == 'h' )
        {
            placemark.setPopularity( 1000000000 );
            placemark.setPopularityIndex( 18 );
        }
        // RoboticRover (r)
        else if ( placemark.role() == 'r' )
        {
            placemark.setPopularity( 10000000 );
            placemark.setPopularityIndex( 16 );
        }
        // UnmannedSoftLandingSite (u)
        else if ( placemark.role() == 'u' )
        {
            placemark.setPopularity( 1000000 );
            placemark.setPopularityIndex( 14 );
        }
        // UnmannedSoftLandingSite (i)
        else if ( placemark.role() == 'i' )
        {
            placemark.setPopularity( 1000000 );
            placemark.setPopularityIndex( 14 );
        }
        // Space Terrain: Craters, Maria, Montes, Valleys, etc.
        else if (    placemark.role() == 'm' || placemark.role() == 'v' 
                  || placemark.role() == 'o' || placemark.role() == 'c'
                  || placemark.role() == 'a' )
        {
            qint64 diameter = placemark.population();
            if ( diameter >= 0 )
            {
                hasPopularity = true;
                placemark.setPopularity( diameter );
                if ( placemark.role() == 'c' ) {
                    placemark.setPopularityIndex( spacePopIdx( diameter ) );
                    if ( placemark.name() == "Tycho" || placemark.name() == "Copernicus" ) {
                        placemark.setPopularityIndex( 17 );
                    }
                }
                else {
                    placemark.setPopularityIndex( spacePopIdx( diameter ) );
                }

                if ( placemark.role() == 'a' && diameter == 0 ) {
                    placemark.setPopularity( 1000000000 );
                    placemark.setPopularityIndex( 18 );
                }
            }
        }
        else
        {
            qint64 population = placemark.population();
            if ( population >= 0 )
            {
                hasPopularity = true;
                placemark.setPopularity( population );
                placemark.setPopularityIndex( cityPopIdx( population ) );
            }
        }

//  Then we set the visual category:

        if ( placemark.role() == 'H' )      placemark.setVisualCategory( GeoDataPlacemark::Mountain );
        else if ( placemark.role() == 'V' ) placemark.setVisualCategory( GeoDataPlacemark::Volcano );

        else if ( placemark.role() == 'm' ) placemark.setVisualCategory( GeoDataPlacemark::Mons );
        else if ( placemark.role() == 'v' ) placemark.setVisualCategory( GeoDataPlacemark::Valley );
        else if ( placemark.role() == 'o' ) placemark.setVisualCategory( GeoDataPlacemark::OtherTerrain );
        else if ( placemark.role() == 'c' ) placemark.setVisualCategory( GeoDataPlacemark::Crater );
        else if ( placemark.role() == 'a' ) placemark.setVisualCategory( GeoDataPlacemark::Mare );

        else if ( placemark.role() == 'P' ) placemark.setVisualCategory( GeoDataPlacemark::GeographicPole );
        else if ( placemark.role() == 'M' ) placemark.setVisualCategory( GeoDataPlacemark::MagneticPole );
        else if ( placemark.role() == 'W' ) placemark.setVisualCategory( GeoDataPlacemark::ShipWreck );
        else if ( placemark.role() == 'F' ) placemark.setVisualCategory( GeoDataPlacemark::AirPort );
        else if ( placemark.role() == 'A' ) placemark.setVisualCategory( GeoDataPlacemark::Observatory );
        else if ( placemark.role() == 'K' ) placemark.setVisualCategory( GeoDataPlacemark::Continent );
        else if ( placemark.role() == 'O' ) placemark.setVisualCategory( GeoDataPlacemark::Ocean );
        else if ( placemark.role() == 'S' ) placemark.setVisualCategory( GeoDataPlacemark::Nation );
        else if ( placemark.role() == 'N' ) placemark.setVisualCategory( 
            ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallCity )
                + ( placemark.popularityIndex() -1 ) / 4 * 4 ) ) );
        else if ( placemark.role() == 'R' ) placemark.setVisualCategory( 
            ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallStateCapital )
                + ( placemark.popularityIndex() -1 ) / 4 * 4 ) ) );
        else if ( placemark.role() == 'C' || placemark.role() == 'B' ) placemark.setVisualCategory( 
            ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallNationCapital )
                + ( placemark.popularityIndex() -1 ) / 4 * 4 ) ) );

        else if ( placemark.role() == ' ' && !hasPopularity && placemark.visualCategory() == GeoDataPlacemark::Unknown ) {
            placemark.setVisualCategory( GeoDataPlacemark::Unknown ); // default location
            placemark.setPopularityIndex(0);
        }
        else if ( placemark.role() == 'h' ) placemark.setVisualCategory( GeoDataPlacemark::MannedLandingSite );
        else if ( placemark.role() == 'r' ) placemark.setVisualCategory( GeoDataPlacemark::RoboticRover );
        else if ( placemark.role() == 'u' ) placemark.setVisualCategory( GeoDataPlacemark::UnmannedSoftLandingSite );
        else if ( placemark.role() == 'i' ) placemark.setVisualCategory( GeoDataPlacemark::UnmannedHardLandingSite );

        if ( placemark.role() == 'W' && placemark.popularityIndex() > 12 )
            placemark.setPopularityIndex( 12 );
        if ( placemark.role() == 'O' )
            placemark.setPopularityIndex( 16 );
        if ( placemark.role() == 'K' )
            placemark.setPopularityIndex( 19 );
        if ( !placemark.isVisible() ) {
            placemark.setPopulation(-1);
        }
        if ( placemark.population() < 0 ) {
            placemark.setPopularityIndex( -1 );
        }
    }

}

int MarblePlacemarkModel::cityPopIdx( qint64 population )
{
    int popidx = 15;

    if ( population < 2500 )        popidx=1;
    else if ( population < 5000)    popidx=2;
    else if ( population < 7500)    popidx=3;
    else if ( population < 10000)   popidx=4;
    else if ( population < 25000)   popidx=5;
    else if ( population < 50000)   popidx=6;
    else if ( population < 75000)   popidx=7;
    else if ( population < 100000)  popidx=8;
    else if ( population < 250000)  popidx=9;
    else if ( population < 500000)  popidx=10;
    else if ( population < 750000)  popidx=11;
    else if ( population < 1000000) popidx=12;
    else if ( population < 2500000) popidx=13;
    else if ( population < 5000000) popidx=14;

    return popidx;
}

int MarblePlacemarkModel::spacePopIdx( qint64 population )
{
    int popidx = 18;

    if ( population < 1000 )        popidx=1;
    else if ( population < 2000)    popidx=2;
    else if ( population < 4000)    popidx=3;
    else if ( population < 6000)    popidx=4;
    else if ( population < 8000)    popidx=5;
    else if ( population < 10000)   popidx=6;
    else if ( population < 20000)   popidx=7;

    else if ( population < 40000  )  popidx=8;
    else if ( population < 60000)    popidx=9;
    else if ( population < 80000  )  popidx=10;
    else if ( population < 100000)   popidx=11;
    else if ( population < 200000 )  popidx=13;
    else if ( population < 400000 )  popidx=15;
    else if ( population < 600000 )  popidx=17;

    return popidx;
}

int MarblePlacemarkModel::areaPopIdx( qreal area )
{
    Q_UNUSED( area );
    int popidx = 17;
    if      ( area <  200000  )      popidx=11;
    else if ( area <  400000  )      popidx=12;
    else if ( area < 1000000  )      popidx=13;
    else if ( area < 2500000  )      popidx=14;
    else if ( area < 5000000  )      popidx=15;
    else if ( area < 10000000 )      popidx=16;

    return popidx;
}

#include "MarblePlacemarkModel.moc"
