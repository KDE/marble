//
// This file is part of the Marble Desktop Globe.
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

namespace Marble {
    
class WikipediaWidget;

class GeonamesParser : public QXmlStreamReader
{
public:
    GeonamesParser( QList<WikipediaWidget *> *list,
                    QObject *parent );

    bool read( QByteArray data );

private:
    void readUnknownElement();
    void readGeonames();
    void readEntry();
    void readTitle( WikipediaWidget *widget );
    void readLongitude( WikipediaWidget *widget );
    void readLatitude( WikipediaWidget *widget );
    void readUrl( WikipediaWidget *widget );
    void readThumbnailImage( WikipediaWidget *widget );

    QList<WikipediaWidget *> *m_list;
    QObject *m_parent;
};

} // Marble namespace

#endif // FLICKRPARSER
