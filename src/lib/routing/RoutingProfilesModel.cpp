//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Niko Sams <niko.sams@gmail.com>
//

#include "RoutingProfilesModel.h"

#include "PluginManager.h"
#include "RunnerPlugin.h"

namespace Marble
{

RoutingProfilesModel::RoutingProfilesModel( PluginManager* pluginManager, QObject *parent )
    : QAbstractListModel( parent ), m_pluginManager( pluginManager )
{
}

QVariant RoutingProfilesModel::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() ) { return QVariant(); }
    if ( index.parent().isValid() ) { return QVariant(); }
    if ( index.row() >= m_profiles.count() ) { return QVariant(); }
    if ( ( role == Qt::DisplayRole || role == Qt::EditRole ) && index.column() == 0 ) {
        return m_profiles.at( index.row() ).name();
    }
    return QVariant();
}

int RoutingProfilesModel::rowCount( const QModelIndex& parent ) const
{
    if ( parent.isValid() ) { return 0; }
    return m_profiles.count();
}

bool RoutingProfilesModel::removeRows( int row, int count, const QModelIndex& parent )
{
    if ( parent.isValid() ) { return false; }
    if ( row + count > m_profiles.count()) { return false; }
    beginRemoveRows( parent, row, row + count );
    for ( int i = 0; i < count; ++i) {
        m_profiles.removeAt( row+i );
    }
    endRemoveRows();
    return true;
}

void RoutingProfilesModel::setProfiles( const QList<RoutingProfile>& profiles )
{ 
    beginResetModel();
    m_profiles = profiles;
    endResetModel();
}

QList<RoutingProfile> RoutingProfilesModel::profiles() const
{
    return m_profiles;
}

void RoutingProfilesModel::addProfile( const QString& name )
{
    beginInsertRows( QModelIndex(), m_profiles.count(), m_profiles.count() );
    m_profiles << RoutingProfile( name );
    endInsertRows();
}

bool RoutingProfilesModel::moveUp( int row )
{
    if ( row < 1 ) { return false; }
    if ( row >= m_profiles.count() ) { return false; }
    if ( !beginMoveRows( QModelIndex(), row, row, QModelIndex(), row-1 ) ) {
        Q_ASSERT( false );
        return false;
    }
    m_profiles.swap( row, row-1 );
    endMoveRows();
    return true;
}

bool RoutingProfilesModel::moveDown( int row )
{
    return moveUp( row + 1 );
}

bool RoutingProfilesModel::setProfileName( int row, const QString& name)
{
    if ( row < 0 ) { return false; }
    if ( row >= m_profiles.count() ) { return false; }
    m_profiles[row].setName( name );
    emit dataChanged( index( row, 0 ), index( row, 0 ) );
    return true;
}

bool RoutingProfilesModel::setProfilePluginSettings( int row, const QHash< QString, QHash< QString, QVariant > >& pluginSettings )
{
    if ( row < 0 ) { return false; }
    if ( row >= m_profiles.count() ) { return false; }
    m_profiles[row].pluginSettings() = pluginSettings;
    return true;
}

QString templateName( RoutingProfilesModel::ProfileTemplate profileTemplate )
{
    switch ( profileTemplate ) {
        case RoutingProfilesModel::CarFastestTemplate:
            return RoutingProfilesModel::tr( "Car (fastest)" );
        case RoutingProfilesModel::CarShortestTemplate:
            return RoutingProfilesModel::tr( "Car (shortest)" );
        case RoutingProfilesModel::CarEcologicalTemplate:
            return RoutingProfilesModel::tr( "Car (ecological)" );
        case RoutingProfilesModel::BicycleTemplate:
            return RoutingProfilesModel::tr( "Bicycle" );
        case RoutingProfilesModel::PedestrianTemplate:
            return RoutingProfilesModel::tr( "Pedestrian" );
        case RoutingProfilesModel::LastTemplate:
            break;
    }
    return RoutingProfilesModel::tr( "Unknown" );
}

void RoutingProfilesModel::loadDefaultProfiles()
{
    beginInsertRows( QModelIndex(), m_profiles.count(), m_profiles.count() + int( LastTemplate ) - 1 );
    for ( int i=0; i < LastTemplate; ++i) {
        ProfileTemplate tpl = static_cast<ProfileTemplate>( i );
        RoutingProfile profile( templateName( tpl ) );
        bool profileSupportedByAtLeastOnePlugin = false;
        foreach( RunnerPlugin* plugin, m_pluginManager->runnerPlugins() ) {
            if ( !plugin->supports( RunnerPlugin::Routing ) ) {
                continue;
            }
            if ( plugin->supportsTemplate( tpl ) ) {
                profileSupportedByAtLeastOnePlugin = true;
                break;
            }
        }
        if ( !profileSupportedByAtLeastOnePlugin ) {
            continue;
        }
        foreach( RunnerPlugin* plugin, m_pluginManager->runnerPlugins() ) {
            if ( !plugin->supports( RunnerPlugin::Routing ) ) {
                continue;
            }
            if ( plugin->supportsTemplate( tpl ) ) {
                profile.pluginSettings()[plugin->nameId()] = plugin->templateSettings( tpl );
            }
        }
        m_profiles << profile;
    }
    endInsertRows();
}

}

#include "RoutingProfilesModel.moc"
