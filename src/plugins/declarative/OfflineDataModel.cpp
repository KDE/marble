//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OfflineDataModel.h"
#include "MarbleDirs.h"

#include <QtCore/QModelIndex>
#include <QtCore/QDir>

OfflineDataModel::OfflineDataModel( QObject *parent ) : QSortFilterProxyModel( parent ),
    m_vehicleTypeFilter( Any )
{
    m_newstuffModel.setTargetDirectory( Marble::MarbleDirs::localPath() + "/maps" );
    m_newstuffModel.setRegistryFile( QDir::homePath() + "/.kde/share/apps/knewstuff3/marble-offline-data.knsregistry", Marble::NewstuffModel::NameTag );
    m_newstuffModel.setProvider( "http://files.kde.org/marble/newstuff/maps-monav.xml" );

    setSourceModel( &m_newstuffModel );
    QHash<int,QByteArray> roleNames = m_newstuffModel.roleNames();
    roleNames[Qt::UserRole+17] = "continent";
    setRoleNames( roleNames );

    sort( 0 );
    setDynamicSortFilter( true );

    connect( &m_newstuffModel, SIGNAL( installationProgressed( int, qreal ) ), this, SLOT( handleInstallationProgress( int,qreal ) ) );
    connect( &m_newstuffModel, SIGNAL( installationFinished( int ) ), this, SLOT( handleInstallationFinished( int ) ) );
    connect( &m_newstuffModel, SIGNAL( installationFailed( int, QString ) ), this, SLOT( handleInstallationFailed( int, QString ) ) );
    connect( &m_newstuffModel, SIGNAL( uninstallationFinished( int ) ), this, SLOT( handleUninstallationFinished( int ) ) );
}

int OfflineDataModel::count()
{
    return rowCount();
}

QVariant OfflineDataModel::data(const QModelIndex &index, int role) const
{
    if ( index.isValid() && index.row() >= 0 && index.row() < rowCount() && role == Qt::DisplayRole ) {
        QStringList const data = QSortFilterProxyModel::data( index, role ).toString().split( '/' );
        if ( data.size() > 1 ) {
            QString result = data.at( 1 );
            for ( int i=2; i<data.size(); ++i ) {
                result += " / " + data.at( i );
            }
            result.remove( QLatin1String( " (Motorcar)" ) );
            result.remove( QLatin1String( " (Pedestrian)" ) );
            result.remove( QLatin1String( " (Bicycle)" ) );
            return result.trimmed();
        }
    }

    if ( index.isValid() && index.row() >= 0 && index.row() < rowCount() && role == Qt::UserRole+17 ) {
        QStringList const data = QSortFilterProxyModel::data( index, Qt::DisplayRole ).toString().split( '/' );
        if ( data.size() > 1 ) {
            return data.first().trimmed();
        }
    }

    return QSortFilterProxyModel::data( index, role );
}

void OfflineDataModel::setVehicleTypeFilter( VehicleTypes filter )
{
    m_vehicleTypeFilter = filter;
    reset();
}

void OfflineDataModel::install( int index )
{
    m_newstuffModel.install( toSource( index ) );
}

void OfflineDataModel::uninstall( int index )
{
    m_newstuffModel.uninstall( toSource( index ) );
}

void OfflineDataModel::cancel( int index )
{
    m_newstuffModel.cancel( toSource( index ) );
}

int OfflineDataModel::fromSource( int index ) const
{
    return mapFromSource( m_newstuffModel.index( index ) ).row();
}

int OfflineDataModel::toSource(int idx) const
{
    return mapToSource( index( idx, 0 ) ).row();
}

void OfflineDataModel::handleInstallationProgress( int index, qreal progress )
{
    emit installationProgressed( fromSource( index ), progress );
}

void OfflineDataModel::handleInstallationFinished( int index )
{
    emit installationFinished( fromSource( index ) );
}

void OfflineDataModel::handleInstallationFailed( int index, const QString &error )
{
    emit installationFailed( fromSource( index ), error );
}

void OfflineDataModel::handleUninstallationFinished( int index )
{
    emit uninstallationFinished( fromSource( index ) );
}

bool OfflineDataModel::filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const
{
    if ( QSortFilterProxyModel::filterAcceptsRow( source_row, source_parent ) ) {
        QModelIndex const index = sourceModel()->index( source_row, 0, source_parent );
        QString const data = sourceModel()->data( index, Qt::DisplayRole ).toString();
        if ( ( m_vehicleTypeFilter & Motorcar ) && data.contains( "(Motorcar)" ) ) {
            return true;
        } else if ( ( m_vehicleTypeFilter & Bicycle ) && data.contains( "(Bicycle)" ) ) {
            return true;
        } else if ( ( m_vehicleTypeFilter & Pedestrian ) && data.contains( "(Pedestrian)" ) ) {
            return true;
        }
    }

    return false;
}

#include "OfflineDataModel.moc"
