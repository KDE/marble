// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef FLICKRPARSER_H
#define FLICKRPARSER_H

#include <QList>
#include <QXmlStreamReader>

class QByteArray;
class QObject;

namespace Marble
{

class MarbleWidget;
class PhotoPluginItem;

class FlickrParser : public QXmlStreamReader
{
public:
    FlickrParser(Marble::MarbleWidget *widget, QList<PhotoPluginItem *> *list, QObject *parent);

    bool read(const QByteArray &data);

private:
    void readUnknownElement();
    void readFlickr();
    void readPhotos();
    void readPhoto();

    MarbleWidget *const m_marbleWidget;
    QList<PhotoPluginItem *> *const m_list;
    QObject *const m_parent;
};

} // Marble namespace

#endif // FLICKRPARSER
