//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_MONAVMAPSMODEL_H
#define MARBLE_MONAVMAPSMODEL_H

#include "MonavMap.h"

#include <QtCore/QAbstractTableModel>

namespace Marble
{

class MonavMapsModel : public QAbstractTableModel
{
public:
    explicit MonavMapsModel( const QVector<MonavMap> &data, QObject * parent = 0 );

    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;

    static bool sortByName( const MonavMap &first, const MonavMap &second );

    void deleteMapFiles( int index );

    QString payload( int index ) const;

    void setInstallableVersions( const QMap<QString, QString> localMaps );

private:
    QVector<MonavMap> m_data;

    QMap<QString, QString> m_remoteMaps;
};

}

#endif // MARBLE_MONAVMAPSMODEL_H
