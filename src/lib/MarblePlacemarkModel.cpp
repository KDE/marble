//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


// Own
#include "MarblePlacemarkModel.h"
#include "MarblePlacemarkModel_P.h"

// Qt
#include <QtCore/QTime>

// Marble
#include "MarbleDebug.h"
#include "GeoDataExtendedData.h"
#include "GeoDataStyle.h"       // In geodata/data/

using namespace Marble;

class MarblePlacemarkModel::Private
{

 public:
    Private()
      : m_size(0)
    {
    }

    ~Private()
    {
    }

    int m_size;
    QVector<GeoDataPlacemark*>     *m_placemarkContainer;
};


// ---------------------------------------------------------------------------


MarblePlacemarkModel::MarblePlacemarkModel( QObject *parent )
    : QAbstractListModel( parent ),
      d( new Private )
{
    QHash<int,QByteArray> roles = roleNames();
    roles[DescriptionRole] = "description";
    roles[LongitudeRole] = "longitude";
    roles[LatitudeRole] = "latitude";
    setRoleNames( roles );
}

MarblePlacemarkModel::~MarblePlacemarkModel()
{
    delete d;
}

void MarblePlacemarkModel::setPlacemarkContainer( QVector<GeoDataPlacemark*> *container )
{
    d->m_placemarkContainer = container;
}

int MarblePlacemarkModel::rowCount( const QModelIndex &parent ) const
{
    if ( !parent.isValid() )
        return d->m_size;
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

QVariant MarblePlacemarkModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( index.row() >= d->m_placemarkContainer->size() )
        return QVariant();

    if ( role == Qt::DisplayRole ) {
        return d->m_placemarkContainer->at( index.row() )->name();
    } else if ( role == Qt::DecorationRole ) {
          return qVariantFromValue( d->m_placemarkContainer->at( index.row() )->style()->iconStyle().icon() );
    } else if ( role == PopularityIndexRole ) {
        return d->m_placemarkContainer->at( index.row() )->zoomLevel();
    } else if ( role == VisualCategoryRole ) {
        return d->m_placemarkContainer->at( index.row() )->visualCategory();
    } else if ( role == AreaRole ) {
        return d->m_placemarkContainer->at( index.row() )->area();
    } else if ( role == PopulationRole ) {
        return d->m_placemarkContainer->at( index.row() )->population();
    } else if ( role == CountryCodeRole ) {
        return d->m_placemarkContainer->at( index.row() )->countryCode();
    } else if ( role == StateRole ) {
        return d->m_placemarkContainer->at( index.row() )->state();
    } else if ( role == PopularityRole ) {
        return d->m_placemarkContainer->at( index.row() )->popularity();
    } else if ( role == DescriptionRole ) {
        return d->m_placemarkContainer->at( index.row() )->description();
    } else if ( role == Qt::ToolTipRole ) {
        return d->m_placemarkContainer->at( index.row() )->description();
    } else if ( role == GeoTypeRole ) {
        return d->m_placemarkContainer->at( index.row() )->role();
    } else if ( role == CoordinateRole ) {
        return qVariantFromValue( d->m_placemarkContainer->at( index.row() )->coordinate() );
    } else if ( role == StyleRole ) {
        return qVariantFromValue( d->m_placemarkContainer->at( index.row() )->style() );
    } else if ( role == GmtRole ) {
        return qVariantFromValue( d->m_placemarkContainer->at( index.row() )->extendedData().value("gmt").value() );
    } else if ( role == DstRole ) {
        return qVariantFromValue( d->m_placemarkContainer->at( index.row() )->extendedData().value("dst").value() );
    } else if ( role == GeometryRole ) {
        return qVariantFromValue( d->m_placemarkContainer->at( index.row() )->geometry() );
    } else if ( role == ObjectPointerRole ) {
        return qVariantFromValue( dynamic_cast<GeoDataObject*>( d->m_placemarkContainer->at( index.row() ) ) );
    } else if ( role == LongitudeRole ) {
        return qVariantFromValue( d->m_placemarkContainer->at( index.row() )->coordinate().longitude( GeoDataCoordinates::Degree ) );
    } else if ( role == LatitudeRole ) {
        return qVariantFromValue( d->m_placemarkContainer->at( index.row() )->coordinate().latitude( GeoDataCoordinates::Degree ) );
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

void MarblePlacemarkModel::addPlacemarks( int start,
                                          int length )
{
    Q_UNUSED(start);

// performance wise a reset is far better when the provided list
// is significant. That is an issue because we have
// MarbleControlBox::m_sortproxy as a sorting customer.
// I leave the balance search as an exercise to the reader...

    QTime t;
    t.start();
//    beginInsertRows( QModelIndex(), start, start + length );
    d->m_size += length;
//    endInsertRows();
    reset();
    emit countChanged();
    mDebug() << "addPlacemarks: Time elapsed:" << t.elapsed() << "ms for" << length << "Placemarks.";
}

void  MarblePlacemarkModel::removePlacemarks( const QString &containerName,
                                              int start,
                                              int length )
{
    if ( length > 0 ) {
        QTime t;
        t.start();
        beginRemoveRows( QModelIndex(), start, start + length );
        d->m_size -= length;
        endRemoveRows();
        emit layoutChanged();
        emit countChanged();
        mDebug() << "removePlacemarks(" << containerName << "): Time elapsed:" << t.elapsed() << "ms for" << length << "Placemarks.";
    }
}

#include "MarblePlacemarkModel.moc"
