// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef GEONAMESPARSER_H
#define GEONAMESPARSER_H

#include <QList>
#include <QXmlStreamReader>

class QByteArray;
class QObject;

namespace Marble
{

class MarbleWidget;
class WikipediaItem;

class GeonamesParser : public QXmlStreamReader
{
public:
    GeonamesParser(MarbleWidget *widget, QList<WikipediaItem *> *list, QObject *parent);

    bool read(const QByteArray &data);

private:
    void readUnknownElement();
    void readGeonames();
    void readEntry();
    void readTitle(WikipediaItem *item);
    void readLongitude(WikipediaItem *item);
    void readLatitude(WikipediaItem *item);
    void readUrl(WikipediaItem *item);
    void readSummary(WikipediaItem *item);
    void readThumbnailImage(WikipediaItem *item);
    void readRank(WikipediaItem *item);

    MarbleWidget *const m_marbleWidget;
    QList<WikipediaItem *> *const m_list;
    QObject *const m_parent;
};

} // Marble namespace

#endif // FLICKRPARSER
