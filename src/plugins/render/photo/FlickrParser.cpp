// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "FlickrParser.h"

// Marble
#include "PhotoPluginItem.h"

// Qt
#include <QByteArray>

using namespace Marble;

FlickrParser::FlickrParser(MarbleWidget *widget, QList<PhotoPluginItem *> *list, QObject *parent)
    : m_marbleWidget(widget)
    , m_list(list)
    , m_parent(parent)
{
}

bool FlickrParser::read(const QByteArray &data)
{
    addData(data);

    while (!atEnd()) {
        readNext();

        if (isStartElement()) {
            if (name() == QLatin1StringView("rsp")) {
                if (attributes().value(QLatin1StringView("stat")) == QLatin1StringView("ok")) {
                    readFlickr();
                } else {
                    raiseError(QObject::tr("Query failed"));
                }
            } else {
                raiseError(QObject::tr("The file is not a valid Flickr answer."));
            }
        }
    }

    return !error();
}

void FlickrParser::readUnknownElement()
{
    Q_ASSERT(isStartElement());

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
            readUnknownElement();
    }
}

void FlickrParser::readFlickr()
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("rsp") && attributes().value(QLatin1StringView("stat")) == QLatin1StringView("ok"));

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1StringView("photos"))
                readPhotos();
            else
                readUnknownElement();
        }
    }
}

void FlickrParser::readPhotos()
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("photos"));

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == QLatin1StringView("photo"))
                readPhoto();
            else
                readUnknownElement();
        }
    }
}

void FlickrParser::readPhoto()
{
    Q_ASSERT(isStartElement() && name() == QLatin1StringView("photo"));

    if (attributes().hasAttribute(QLatin1StringView("id"))) {
        auto item = new PhotoPluginItem(m_marbleWidget, m_parent);
        item->setId(attributes().value(QLatin1StringView("id")).toString());
        item->setServer(attributes().value(QLatin1StringView("server")).toString());
        item->setFarm(attributes().value(QLatin1StringView("farm")).toString());
        item->setSecret(attributes().value(QLatin1StringView("secret")).toString());
        item->setOwner(attributes().value(QLatin1StringView("owner")).toString());
        item->setTitle(attributes().value(QLatin1StringView("title")).toString());
        m_list->append(item);
    }

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
            break;
    }
}
