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

#include <QtCore/QDebug>
#include <QtCore/QModelIndex>
#include <QtGui/QPixmap>

#include "geodata/data/GeoDataStyle.h"

#include "PlaceMarkContainer.h"
#include "PlaceMarkManager.h"

#include "PlaceMarkModel.h"
#include "PlaceMarkModel_P.h"

class PlaceMarkModel::Private
{
    friend class PlaceMarkManager;

 public:
    Private( PlaceMarkManager *manager, PlaceMarkModel *parent )
        : m_parent( parent ), m_manager( manager )
    {
        Q_ASSERT( m_manager != 0 );
    }

    ~Private()
    {
    }

    PlaceMarkModel* m_parent;
    PlaceMarkManager* m_manager;
    PlaceMarkContainer m_placeMarkContainer;
};

// ---------------------------------------------------------------------------

PlaceMarkModel::PlaceMarkModel( PlaceMarkManager *manager, QObject *parent )
    : QAbstractListModel( parent ),
      d( new Private( manager, this ) )
{
    // Register at PlaceMarkManager
    d->m_manager->setPlaceMarkModel( this );
}

PlaceMarkModel::~PlaceMarkModel()
{
    // Unregister from PlaceMarkManager
    d->m_manager->setPlaceMarkModel( 0 );

    delete d;
}


int PlaceMarkModel::rowCount( const QModelIndex &parent ) const
{
    if ( !parent.isValid() )
        return d->m_placeMarkContainer.size();
    else
        return 0;
}

int PlaceMarkModel::columnCount( const QModelIndex &parent ) const
{
    if ( !parent.isValid() )
        return 1;
    else
        return 0;
}

QVariant PlaceMarkModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( index.row() >= d->m_placeMarkContainer.size() )
        return QVariant();

    if ( role == Qt::DisplayRole ) {
        return d->m_placeMarkContainer.at( index.row() )->name();
    } else if ( role == Qt::DecorationRole ) {
        GeoDataStyle* style = (GeoDataStyle*)( d->m_placeMarkContainer.at( index.row() )->style() );
        QVariant v;
        v.setValue( style->iconStyle()->icon() );
        return v;
    } else if ( role == GeoTypeRole ) {
        return d->m_placeMarkContainer.at( index.row() )->role();
    } else if ( role == PopularityIndexRole ) {
        return d->m_placeMarkContainer.at( index.row() )->popularityIndex();
    } else if ( role == CoordinateRole ) {
        QVariant v;
        v.setValue( d->m_placeMarkContainer.at( index.row() )->coordinate() );
        return v;
    } else if ( role == CountryCodeRole ) {
        return d->m_placeMarkContainer.at( index.row() )->countryCode();
    } else if ( role == VisualCategoryRole ) {
        return d->m_placeMarkContainer.at( index.row() )->visualCategory();
    } else if ( role == StyleRole ) {
        GeoDataStyle* s = d->m_placeMarkContainer.at( index.row() )->style();
        QVariant v;
        v.setValue( s );
        return v;
    } else if ( role == PopularityRole ) {
        return d->m_placeMarkContainer.at( index.row() )->popularity();
    } else if ( role == DescriptionRole ) {
        return d->m_placeMarkContainer.at( index.row() )->description();
    } else
        return QVariant();
}

QModelIndexList PlaceMarkModel::match( const QModelIndex & start, int role, 
                                       const QVariant & value, int hits,
                                       Qt::MatchFlags flags ) const
{
    QList<QModelIndex> results;

    int      count = 0;
    QString  listName;
    QString  queryString;
    QString  simplifiedListName;
    int      row = start.row();

    while ( row < rowCount() && count != hits ) {
        if ( flags & Qt::MatchStartsWith ) {
            listName    = data( index( row, 0 ), role ).toString();
            queryString = value.toString();
            simplifiedListName = GeoString::deaccent( listName );

            if ( listName.startsWith( queryString, Qt::CaseInsensitive ) 
                 || listName.remove( QChar( '\'' ), Qt::CaseSensitive ).startsWith( queryString )
                 || listName.replace( QChar( '-' ), QChar( ' ' ) ).startsWith( queryString )
                 || GeoString::deaccent( simplifiedListName ).startsWith( queryString )
                 )
            {
                results << index( row, 0 );

                ++count;
            }
        }

        ++row;
    }

    return results;
}

void PlaceMarkModel::addPlaceMarks( const PlaceMarkContainer &placeMarks, bool clearPrevious )
{
  // For now we simply remove any previous placemarks
    if ( clearPrevious == true )
    {
        qDeleteAll(d->m_placeMarkContainer.begin(), d->m_placeMarkContainer.end());
        d->m_placeMarkContainer.clear();
    }

    d->m_placeMarkContainer << placeMarks;
    d->m_placeMarkContainer.sort();

    reset();
}

void PlaceMarkModel::clearPlaceMarks()
{
  qDeleteAll( d->m_placeMarkContainer );
  d->m_placeMarkContainer.clear();

  reset();
}

#include "PlaceMarkModel.moc"
