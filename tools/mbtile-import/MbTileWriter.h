// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_MBTILEWRITER_H
#define MARBLE_MBTILEWRITER_H

#include <QFileInfo>
#include <QSqlQuery>

namespace Marble
{

class MbTileWriter
{
public:
    explicit MbTileWriter(const QString &filename, const QString &extension = "o5m");
    ~MbTileWriter();

    void setOverwriteTiles(bool overwrite);
    void setReportProgress(bool report);
    void setCommitInterval(int interval);

    void addTile(const QFileInfo &file, qint32 x, qint32 y, qint32 z);
    void addTile(QIODevice *device, qint32 x, qint32 y, qint32 z);
    bool hasTile(qint32 x, qint32 y, qint32 z) const;

private:
    void execQuery(const QString &query) const;
    void execQuery(QSqlQuery &query) const;
    void setMetaData(const QString &name, const QString &value);

    bool m_overwriteTiles;
    bool m_reportProgress;
    int m_tileCounter;
    int m_commitInterval;
};

}

#endif
