//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "EclipsesModel.h"

#include "EclipsesItem.h"
#include "MarbleDebug.h"
#include "MarbleClock.h"

#include "ecl/eclsolar.h"

namespace Marble
{

EclipsesModel::EclipsesModel( const MarbleModel *model, QObject *parent )
    : QAbstractItemModel( parent ),
      m_marbleModel( model ),
      m_currentYear( 0 )
{
    m_ecps = new EclSolar();
    m_ecps->setLunarEcl( false );
    m_ecps->setTimezone( model->clock()->timezone() / 3600. );

    // oberservation point defaults to home location
    qreal lon, lat;
    int zoom;
    m_marbleModel->home( lon, lat, zoom );
    GeoDataCoordinates homeLocation( lon, lat, 0, GeoDataCoordinates::Degree );
    setObservationPoint( homeLocation );
}

EclipsesModel::~EclipsesModel()
{
    clear();
    delete m_ecps;
}
const GeoDataCoordinates& EclipsesModel::observationPoint() const
{
    return m_observationPoint;
}

void EclipsesModel::setObservationPoint( const GeoDataCoordinates &coords )
{
    m_observationPoint = coords;
    m_ecps->setLocalPos( coords.latitude(), coords.altitude(), 6000. );
}

void EclipsesModel::setYear( int year )
{
    if( m_currentYear != year ) {

        mDebug() << "Year changed - Calculating eclipses...";
        m_currentYear = year;
        m_ecps->putYear( year );

        update();
    }
}

int EclipsesModel::year() const
{
    return m_currentYear;
}

EclipsesItem* EclipsesModel::eclipseWithIndex( int index )
{
    foreach( EclipsesItem *item, m_items ) {
        if( item->index() == index ) {
            return item;
        }
    }

    return NULL;
}

QList<EclipsesItem*> EclipsesModel::items() const
{
    return m_items;
}

QModelIndex EclipsesModel::index( int row, int column, const QModelIndex &parent ) const
{
    if( !hasIndex( row, column, parent ) ) {
        return QModelIndex();
    }

    if( row >= m_items.count() ) {
        return QModelIndex();
    }

    return createIndex( row, column, m_items.at( row ) );
}

QModelIndex EclipsesModel::parent( const QModelIndex &index ) const
{
    Q_UNUSED( index );
    return QModelIndex(); // no parents
}

int EclipsesModel::rowCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return m_items.count();
}

int EclipsesModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return 4; // start, end, type magnitude
}

QVariant EclipsesModel::data( const QModelIndex &index, int role ) const
{
    if( !index.isValid() ) {
        return QVariant();
    }

    if( role != Qt::DisplayRole ) {
        return QVariant();
    }

    Q_ASSERT( index.column() < 4 );

    EclipsesItem *item = static_cast<EclipsesItem*>( index.internalPointer() );
    switch( index.column() ) {
        case 0: return QVariant( item->startDatePartial() );
        case 1: return QVariant( item->endDatePartial() );
        case 2: return QVariant( item->phaseText() );
        case 3: return QVariant( item->magnitude() );
        default: break; // should never happen
    }

    return QVariant();
}

QVariant EclipsesModel::headerData( int section, Qt::Orientation orientation,
                                    int role ) const
{
    if( orientation != Qt::Horizontal || role != Qt::DisplayRole ) {
        return QVariant();
    }

    switch( section ) {
        case 0: return QVariant( tr( "Start" ) );
        case 1: return QVariant( tr( "End" ) );
        case 2: return QVariant( tr( "Type" ) );
        case 3: return QVariant( tr( "Magnitude" ) );
        default: break;
    }

    return QVariant();
}

void EclipsesModel::addItem( EclipsesItem *item )
{
    m_items.append( item );
}

void EclipsesModel::clear()
{
    beginResetModel();

    qDeleteAll( m_items );
    m_items.clear();

    endResetModel();
}

void EclipsesModel::update()
{
    clear();

    beginInsertRows( QModelIndex(), 0, rowCount() );

    int num = m_ecps->getNumberEclYear();
    for( int i = 1; i <= num; ++i ) {
        EclipsesItem *item = new EclipsesItem( m_ecps, i );
        addItem( item );
        mDebug() << "Eclipse" << i << "added:" << item->dateMaximum()
            << "(" << item->phaseText() << ")";
    }

    endInsertRows();
}

} // namespace Marble

#include "EclipsesModel.moc"

