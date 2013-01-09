//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_OFFLINEDATAMODEL_H
#define MARBLE_DECLARATIVE_OFFLINEDATAMODEL_H

#include "NewstuffModel.h"

#include <QtGui/QSortFilterProxyModel>

class OfflineDataModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY( int count READ count NOTIFY countChanged )

    Q_FLAGS(VehicleType VehicleTypes)

public:
    enum VehicleType {
        None = 0x0,
        Motorcar = 0x1,
        Bicycle = 0x2,
        Pedestrian = 0x4,
        Any = Motorcar | Bicycle | Pedestrian
    };

    Q_DECLARE_FLAGS(VehicleTypes, VehicleType)

    explicit OfflineDataModel( QObject* parent = 0 );

    /** @todo FIXME https://bugreports.qt.nokia.com/browse/QTCOMPONENTS-1206 */
    int count();

    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

public Q_SLOTS:
    void setVehicleTypeFilter( VehicleTypes filter );

    void install( int index );

    void uninstall( int index );

    void cancel( int index );

Q_SIGNALS:
    void countChanged();

    void installationProgressed( int newstuffindex, qreal progress );

    void installationFinished( int newstuffindex );

    void installationFailed( int newstuffindex, const QString &error );

    void uninstallationFinished( int newstuffindex );

protected:
    virtual bool filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const;

private Q_SLOTS:
    void handleInstallationProgress( int index, qreal progress );

    void handleInstallationFinished( int index );

    void handleInstallationFailed( int index, const QString &error );

    void handleUninstallationFinished( int index );

private:
    int fromSource( int idx ) const;

    int toSource( int idx ) const;

    Marble::NewstuffModel m_newstuffModel;

    VehicleTypes m_vehicleTypeFilter;
};

#endif
