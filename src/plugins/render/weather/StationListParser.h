// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef STATIONLISTPARSER_H
#define STATIONLISTPARSER_H

// Qt
#include <QList>
#include <QThread>
#include <QXmlStreamReader>

class QString;

namespace Marble
{

class BBCStation;

class StationListParser : public QThread, public QXmlStreamReader
{
    Q_OBJECT
public:
    explicit StationListParser(QObject *parent);
    ~StationListParser() override;

    void read();

    QList<BBCStation> stationList() const;

    void setPath(const QString &path);

protected:
    void run() override;

private:
    void readUnknownElement();
    void readStationList();
    void readStation();
    QString readCharacters();
    void readPoint(BBCStation *station);

    QString m_path;
    QList<BBCStation> m_list;
};

} // Marble namespace

#endif // STATIONLISTPARSER_H
