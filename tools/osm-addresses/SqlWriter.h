//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef SQLWRITER_H
#define SQLWRITER_H

#include "Writer.h"

#include <QPair>
#include <QHash>
#include <QSqlQuery>

namespace Marble
{

class SqlWriter : public Writer
{
public:
    explicit SqlWriter( const QString &filename, QObject* parent = 0 );

    ~SqlWriter() override;

    void addOsmRegion( const OsmRegion &region ) override;

    void addOsmPlacemark( const OsmPlacemark &placemark ) override;

    void saveDatabase( const QString &filename ) const;

private:
    void execQuery( QSqlQuery &query ) const;

    void execQuery( const QString &query ) const;

    QHash<QString, int> m_placemarks;

    QPair<int, QString> m_lastPlacemark;

    int m_placemarkId;
};

}

#endif // SQLWRITER_H
