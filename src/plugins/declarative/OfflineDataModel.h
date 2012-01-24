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

public:
    OfflineDataModel( QObject* parent = 0 );

    /** @todo FIXME https://bugreports.qt.nokia.com/browse/QTCOMPONENTS-1206 */
    int count();

    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

public Q_SLOTS:
    void install( int index );

    void uninstall( int index );

Q_SIGNALS:
    void countChanged();

    void installationProgressed( int index, qreal progress );

    void installationFinished( int index );

    void installationFailed( int index, const QString &error );

    void uninstallationFinished( int index );

private Q_SLOTS:
    void handleInstallationProgress( int index, qreal progress );

    void handleInstallationFinished( int index );

    void handleInstallationFailed( int index, const QString &error );

    void handleUninstallationFinished( int index );

private:
    int fromSource( int idx ) const;

    Marble::NewstuffModel m_newstuffModel;
};

#endif
