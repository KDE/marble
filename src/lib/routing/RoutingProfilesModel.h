//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Niko Sams <niko.sams@gmail.com>
//


#ifndef MARBLE_ROUTINGPROFILESMODEL_H
#define MARBLE_ROUTINGPROFILESMODEL_H

#include "marble_export.h"
#include "RoutingProfile.h"

#include <QAbstractListModel>


namespace Marble {

class PluginManager;

class MARBLE_EXPORT RoutingProfilesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit RoutingProfilesModel( PluginManager *pluginManager, QObject *parent = 0  );

    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    virtual bool removeRows( int row, int count, const QModelIndex& parent = QModelIndex() );

    enum ProfileTemplate {
        CarFastestTemplate,
        CarShortestTemplate,
        CarEcologicalTemplate,
        BicycleTemplate,
        PedestrianTemplate,

        LastTemplate
    };

    void setProfiles( const QList<RoutingProfile> &profiles );
    QList<RoutingProfile> profiles() const;

    void loadDefaultProfiles();

    void addProfile( const QString &name );
    bool moveUp( int row );
    bool moveDown( int row );

    bool setProfileName( int row, const QString &name );
    bool setProfilePluginSettings( int row, const QHash<QString, QHash<QString, QVariant> > &pluginSettings );

private:
    QList<RoutingProfile> m_profiles;
    PluginManager* m_pluginManager;
};

}

#endif // MARBLE_ROUTINGPROFILESMODEL_H
