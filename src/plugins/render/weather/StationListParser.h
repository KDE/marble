//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef STATIONLISTPARSER_H
#define STATIONLISTPARSER_H

// Qt
#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtCore/QList>
#include <QtCore/QThread>
#include <QtCore/QXmlStreamReader>

class QString;

namespace Marble
{

class BBCStation;
    
class StationListParser : public QThread, public QXmlStreamReader
{
    Q_OBJECT
public:
    explicit StationListParser( QObject *parent );
    ~StationListParser();

    void read();

    QList<BBCStation> stationList() const;

    void setPath( QString path );

protected:
    void run();

private:
    void readUnknownElement();
    void readStationList();
    void readStation();
    QString readCharacters();
    void readPoint( BBCStation *station );

    QString m_path;
    QList<BBCStation> m_list;
    QObject *m_parent;
};

} // Marble namespace

#endif // STATIONLISTPARSER_H
