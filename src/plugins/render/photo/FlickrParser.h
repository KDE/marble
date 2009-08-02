//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef FLICKRPARSER_H
#define FLICKRPARSER_H

#include <QtCore/QList>
#include <QtCore/QXmlStreamReader>

class QByteArray;
class QObject;

namespace Marble
{
    
class AbstractDataPluginItem;

class FlickrParser : public QXmlStreamReader
{
public:
    FlickrParser( QList<AbstractDataPluginItem *> *list,
                  QObject *parent );

    bool read( QByteArray data );

private:
    void readUnknownElement();
    void readFlickr();
    void readPhotos();
    void readPhoto();

    QList<AbstractDataPluginItem *> *m_list;
    QObject *m_parent;
};

} // Marble namespace

#endif // FLICKRPARSER
