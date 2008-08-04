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
#include <QtGui/QPixmap>

// Marble
#include "GeoDataStyle.h"       // In geodata/data/
#include "PlaceMarkContainer.h"
#include "PlaceMarkManager.h"


class MarblePlacemarkModel::Private
{
    friend class PlaceMarkManager;

 public:
    Private( PlaceMarkManager *manager, MarblePlacemarkModel *parent )
        : m_parent( parent ), m_manager( manager )
    {
    }

    ~Private()
    {
    }

    MarblePlacemarkModel  *m_parent;
    PlaceMarkManager      *m_manager;
    PlaceMarkContainer     m_placeMarkContainer;
};


// ---------------------------------------------------------------------------


MarblePlacemarkModel::MarblePlacemarkModel( PlaceMarkManager *manager, 
					    QObject *parent )
    : QAbstractListModel( parent ),
      d( new Private( manager, this ) )
{
    // Register at PlaceMarkManager
    if ( d->m_manager )
        d->m_manager->setPlaceMarkModel( this );
}

MarblePlacemarkModel::~MarblePlacemarkModel()
{
    clearPlaceMarks();
    // Unregister from PlaceMarkManager
    if ( d->m_manager )
        d->m_manager->setPlaceMarkModel( 0 );

    delete d;
}

QList<QPersistentModelIndex> MarblePlacemarkModel::persistentIndexList () const
{
    QList<QPersistentModelIndex> modelIndexList;
    const int constRowCount = rowCount();

    for ( int i = 0; i < constRowCount; ++i )
    {
        modelIndexList << index( i, 0 );
    }
    return modelIndexList;
}

int MarblePlacemarkModel::rowCount( const QModelIndex &parent ) const
{
    if ( !parent.isValid() )
        return d->m_placeMarkContainer.size();
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
    return d->m_placeMarkContainer.at( index.row() )->coordinate();
}

GeoDataStyle* MarblePlacemarkModel::styleData( const QModelIndex &index ) const
{
    if ( !index.isValid() )
        return 0;
    else {
        GeoDataFeature* feature = d->m_placeMarkContainer.at( index.row() );
        return feature->style();
    }
}

QVariant MarblePlacemarkModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( index.row() >= d->m_placeMarkContainer.size() )
        return QVariant();

    if ( role == Qt::DisplayRole ) {
        return d->m_placeMarkContainer.at( index.row() )->name();
    } else if ( role == Qt::DecorationRole ) {
          return qVariantFromValue( d->m_placeMarkContainer.at( index.row() )->style()->iconStyle()->icon() );
    } else if ( role == PopularityIndexRole ) {
        return d->m_placeMarkContainer.at( index.row() )->popularityIndex();
    } else if ( role == VisualCategoryRole ) {
        return d->m_placeMarkContainer.at( index.row() )->visualCategory();
    } else if ( role == AreaRole ) {
        return d->m_placeMarkContainer.at( index.row() )->area();
    } else if ( role == PopulationRole ) {
        return d->m_placeMarkContainer.at( index.row() )->population();
    } else if ( role == CountryCodeRole ) {
        return d->m_placeMarkContainer.at( index.row() )->countryCode();
    } else if ( role == PopularityRole ) {
        return d->m_placeMarkContainer.at( index.row() )->popularity();
    } else if ( role == DescriptionRole ) {
        return d->m_placeMarkContainer.at( index.row() )->description();
    } else if ( role == GeoTypeRole ) {
        return d->m_placeMarkContainer.at( index.row() )->role();
    } else if ( role == CoordinateRole ) {
        return qVariantFromValue( d->m_placeMarkContainer.at( index.row() )->coordinate() );
    } else if ( role == StyleRole ) {
        return qVariantFromValue( d->m_placeMarkContainer.at( index.row() )->style() );
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

void MarblePlacemarkModel::addPlaceMarks( PlaceMarkContainer &placeMarks, bool clearPrevious )
{
  // For now we simply remove any previous placemarks
    if ( clearPrevious ) {
        qDeleteAll( d->m_placeMarkContainer.begin(), d->m_placeMarkContainer.end() );
        d->m_placeMarkContainer.clear();
    }

    createFilterProperties( placeMarks );

    d->m_placeMarkContainer << placeMarks;
    d->m_placeMarkContainer.sort();

    reset();
}

void MarblePlacemarkModel::clearPlaceMarks()
{
    qDeleteAll( d->m_placeMarkContainer );
    d->m_placeMarkContainer.clear();

    reset();
}

void MarblePlacemarkModel::createFilterProperties( PlaceMarkContainer &container )
{

    QVector<GeoDataPlacemark*>::Iterator i;
    for ( i = container.begin(); i != container.constEnd(); ++i ) {
        GeoDataPlacemark* placemark = *i;

        bool hasPopularity = false;

        // Mountain (H), Volcano (V), Shipwreck (W)
        if ( placemark->role() == 'H' || placemark->role() == 'V' || placemark->role() == 'W' )
        {
            double altitude = placemark->coordinate().altitude();
            if ( altitude != 0.0 )
            {
                hasPopularity = true;
                placemark->setPopularity( (qint64)(altitude * 1000.0) );
                placemark->setPopularityIndex( cityPopIdx( qAbs( (qint64)(altitude * 1000.0) ) ) );
            }
        }
        // Continent (K), Ocean (O), Nation (S)
        else if ( placemark->role() == 'K' || placemark->role() == 'O' || placemark->role() == 'S' )
        {
            double area = placemark->area();
            if ( area >= 0.0 )
            {
                hasPopularity = true;
//                qDebug() << placemark->name() << " " << (qint64)(area);
                placemark->setPopularity( (qint64)(area * 100) );
                placemark->setPopularityIndex( areaPopIdx( area ) );
            }
        }
        // Pole (P)
        else if ( placemark->role() == 'P' )
        {
            placemark->setPopularity( 1000000000 );
            placemark->setPopularityIndex( 18 );
        }
        // Magnetic Pole (M)
        else if ( placemark->role() == 'M' )
        {
            placemark->setPopularity( 10000000 );
            placemark->setPopularityIndex( 13 );
        }
        else
        {
            qint64 population = placemark->population();
            if ( population >= 0 )
            {
                hasPopularity = true;
                placemark->setPopularity( population );
                placemark->setPopularityIndex( cityPopIdx( population ) );
            }
        }

//  Then we set the visual category:

        if ( placemark->role() == 'H' )      placemark->setVisualCategory( GeoDataPlacemark::Mountain );
        else if ( placemark->role() == 'V' ) placemark->setVisualCategory( GeoDataPlacemark::Volcano );
        else if ( placemark->role() == 'P' ) placemark->setVisualCategory( GeoDataPlacemark::GeographicPole );
        else if ( placemark->role() == 'M' ) placemark->setVisualCategory( GeoDataPlacemark::MagneticPole );
        else if ( placemark->role() == 'W' ) placemark->setVisualCategory( GeoDataPlacemark::ShipWreck );
        else if ( placemark->role() == 'F' ) placemark->setVisualCategory( GeoDataPlacemark::AirPort );
        else if ( placemark->role() == 'K' ) placemark->setVisualCategory( GeoDataPlacemark::Continent );
        else if ( placemark->role() == 'O' ) placemark->setVisualCategory( GeoDataPlacemark::Ocean );
        else if ( placemark->role() == 'S' ) placemark->setVisualCategory( GeoDataPlacemark::Nation );
        else if ( placemark->role() == 'N' ) placemark->setVisualCategory( 
            ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallCity )
                + ( placemark->popularityIndex() -1 ) / 4 * 4 ) ) );
        else if ( placemark->role() == 'R' ) placemark->setVisualCategory( 
            ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallStateCapital )
                + ( placemark->popularityIndex() -1 ) / 4 * 4 ) ) );
        else if ( placemark->role() == 'C' || placemark->role() == 'B' ) placemark->setVisualCategory( 
            ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallNationCapital )
                + ( placemark->popularityIndex() -1 ) / 4 * 4 ) ) );

        else if ( placemark->role() == ' ' && !hasPopularity )
            placemark->setVisualCategory( GeoDataPlacemark::Default ); // default location

        if ( placemark->role() == 'W' && placemark->popularityIndex() > 12 )
            placemark->setPopularityIndex( 12 );
        if ( placemark->role() == 'O' )
            placemark->setPopularityIndex( 16 );
        if ( placemark->role() == 'K' )
            placemark->setPopularityIndex( 19 );

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

int MarblePlacemarkModel::areaPopIdx( double area )
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
