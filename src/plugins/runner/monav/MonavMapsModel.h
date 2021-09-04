//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_MONAVMAPSMODEL_H
#define MARBLE_MONAVMAPSMODEL_H

#include "MonavMap.h"

#include <QAbstractTableModel>

namespace Marble
{

class MonavMapsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MonavMapsModel( const QVector<MonavMap> &data, QObject * parent = nullptr );

    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const override;

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const override;

    int columnCount ( const QModelIndex & parent = QModelIndex() ) const override;

    static bool sortByName( const MonavMap &first, const MonavMap &second );

    void deleteMapFiles( int index );

    QString payload( int index ) const;

    void setInstallableVersions( const QMap<QString, QString> &remoteMaps );

private:
    QVector<MonavMap> m_data;

    QMap<QString, QString> m_remoteMaps;
};

}

#endif // MARBLE_MONAVMAPSMODEL_H
