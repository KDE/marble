//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef GEONAMESPARSER_H
#define GEONAMESPARSER_H

#include <QtCore/QList>
#include <QtCore/QXmlStreamReader>

class QByteArray;
class QObject;

namespace Marble
{
    
class WikipediaItem;

class GeonamesParser : public QXmlStreamReader
{
public:
    GeonamesParser( QList<WikipediaItem *> *list,
                    QObject *parent );

    bool read( const QByteArray& data );

private:
    void readUnknownElement();
    void readGeonames();
    void readEntry();
    void readTitle( WikipediaItem *item );
    void readLongitude( WikipediaItem *item );
    void readLatitude( WikipediaItem *item );
    void readUrl( WikipediaItem *item );
    void readSummary( WikipediaItem *item );
    void readThumbnailImage( WikipediaItem *item );
    void readRank( WikipediaItem *item );

    QList<WikipediaItem *> *m_list;
    QObject *m_parent;
};

} // Marble namespace

#endif // FLICKRPARSER
