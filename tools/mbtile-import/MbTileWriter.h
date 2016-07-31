//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_MBTILEWRITER_H
#define MARBLE_MBTILEWRITER_H

#include <QSqlQuery>
#include <QFileInfo>

namespace Marble
{

class MbTileWriter
{
public:
    explicit MbTileWriter(const QString &filename, const QString &extension="o5m");
    ~MbTileWriter();

    void setOverwriteTiles(bool overwrite);
    void setReportProgress(bool report);

    void addTile(const QFileInfo &file, qint32 x, qint32 y, qint32 z);

private:
    bool haveTile(qint32 x, qint32 y, qint32 z) const;
    void execQuery(const QString &query) const;
    void execQuery(QSqlQuery &query) const;
    void setMetaData(const QString &name, const QString &value);

    bool m_overwriteTiles;
    bool m_reportProgress;
};

}

#endif
