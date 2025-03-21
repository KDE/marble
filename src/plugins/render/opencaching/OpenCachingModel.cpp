// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Daniel Marth <danielmarth@gmx.at>
//

#include "OpenCachingModel.h"
#include "OpenCachingItem.h"

#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleDebug.h"
#include "MarbleGlobal.h"
#include "MarbleModel.h"
#include "OpenCachingCache.h"

#include <QDebug>
#include <QString>
#include <QUrl>
#include <QXmlStreamReader>

namespace Marble
{

class OpenCachingModelPrivate
{
public:
    QHash<QString, QVariant> parseCache(QXmlStreamReader &reader);
    QHash<QString, QVariant> parseLogEntry(QXmlStreamReader &reader);
    QHash<QString, QVariant> parseDescription(QXmlStreamReader &reader);
};

QHash<QString, QVariant> OpenCachingModelPrivate::parseCache(QXmlStreamReader &reader)
{
    QHash<QString, QVariant> cache;
    while (!reader.atEnd()) {
        if (reader.isStartElement() && reader.name() != QLatin1StringView("cache")) {
            if (reader.name() == QLatin1StringView("id")) {
                cache["id"] = reader.attributes().value("id").toString();
            } else if (reader.name() != QLatin1StringView("attributes") && reader.name() != QLatin1StringView("attribute")) {
                cache[reader.name().toString()] = reader.readElementText();
            }
        } else if (reader.isEndElement() && reader.name() == QLatin1StringView("cache")) {
            return cache;
        }
        reader.readNext();
    }
    return QHash<QString, QVariant>();
}

QHash<QString, QVariant> OpenCachingModelPrivate::parseLogEntry(QXmlStreamReader &reader)
{
    QHash<QString, QVariant> cacheLogEntry;
    while (!reader.atEnd()) {
        if (reader.isStartElement() && reader.name() != QLatin1StringView("cachelog")) {
            if (reader.name() == QLatin1StringView("cacheid")) {
                cacheLogEntry["cacheid"] = reader.attributes().value("id").toString();
            } else {
                cacheLogEntry[reader.name().toString()] = reader.readElementText();
            }
        } else if (reader.isEndElement() && reader.name() == QLatin1StringView("cachelog")) {
            return cacheLogEntry;
        }
        reader.readNext();
    }
    return QHash<QString, QVariant>();
}

QHash<QString, QVariant> OpenCachingModelPrivate::parseDescription(QXmlStreamReader &reader)
{
    QHash<QString, QVariant> cacheDesc;
    while (!reader.atEnd()) {
        if (reader.isStartElement() && reader.name() != QLatin1StringView("cachedesc")) {
            if (reader.name() == QLatin1StringView("cacheid")) {
                cacheDesc["cacheid"] = reader.attributes().value("id").toString();
            } else {
                cacheDesc[reader.name().toString()] = reader.readElementText();
            }
        } else if (reader.isEndElement() && reader.name() == QLatin1StringView("cachedesc")) {
            return cacheDesc;
        }
        reader.readNext();
    }
    return QHash<QString, QVariant>();
}

OpenCachingModel::OpenCachingModel(const PluginManager *pluginManager, QObject *parent)
    : AbstractDataPluginModel("opencaching", pluginManager, parent)
    , m_numResults(numberOfItemsOnScreen)
    , m_maxDistance(20)
    , m_minDifficulty(0.0)
    , m_maxDifficulty(5.0)
    , m_startDate(QDateTime::fromString("2006-01-01", "yyyy-MM-dd"))
    , m_endDate(QDateTime::currentDateTime())
    , d(new OpenCachingModelPrivate)
{
}

OpenCachingModel::~OpenCachingModel()
{
}

void OpenCachingModel::setNumResults(int numResults)
{
    m_numResults = numResults;
}

void OpenCachingModel::setMaxDistance(int maxDistance)
{
    m_maxDistance = maxDistance;
}

void OpenCachingModel::setMinDifficulty(double minDifficulty)
{
    m_minDifficulty = minDifficulty;
}

void OpenCachingModel::setMaxDifficulty(double maxDifficulty)
{
    m_maxDifficulty = maxDifficulty;
}

void OpenCachingModel::setStartDate(const QDateTime &startDate)
{
    m_startDate = startDate;
}

void OpenCachingModel::setEndDate(const QDateTime &endDate)
{
    m_endDate = endDate;
}

void OpenCachingModel::getAdditionalItems(const GeoDataLatLonAltBox &box, const MarbleModel *model, qint32 number)
{
    Q_UNUSED(number);

    if (model->planetId() != QLatin1StringView("earth")) {
        return;
    }

    // https://www.opencaching.de/doc/xml/xml11.htm
    const QString openCachingUrl(QLatin1StringView("http://www.opencaching.de/xml/ocxml11.php") + QLatin1StringView("?modifiedsince=")
                                 + m_startDate.toString("yyyyMMddhhmmss") + QLatin1StringView("&cache=1&cachedesc=1&picture=0&cachelog=1&removedobject=0")
                                 + QLatin1StringView("&lat=") + QString::number(box.center().latitude() * RAD2DEG) + QLatin1StringView("&lon=")
                                 + QString::number(box.center().longitude() * RAD2DEG) + QLatin1StringView("&distance=") + QString::number(m_maxDistance)
                                 + QLatin1StringView("&charset=utf-8&cdata=0&session=0&zip=0"));
    downloadDescriptionFile(QUrl(openCachingUrl));
}

void OpenCachingModel::parseFile(const QByteArray &file)
{
    QXmlStreamReader reader(file);
    QXmlStreamReader::TokenType token;
    QHash<int, OpenCachingCache> caches;
    QHash<int, QHash<QString, OpenCachingCacheDescription>> descriptions;
    QHash<int, OpenCachingCacheLog> logs;

    while (!reader.atEnd() && !reader.hasError()) {
        token = reader.readNext();
        if (token == QXmlStreamReader::StartDocument) {
            continue;
        }
        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == QLatin1StringView("cache")) {
                OpenCachingCache cache = d->parseCache(reader);
                caches[cache.id()] = cache;
            } else if (reader.name() == QLatin1StringView("cachedesc")) {
                OpenCachingCacheDescription description = d->parseDescription(reader);
                descriptions[description.cacheId()][description.language()] = description;
            } else if (reader.name() == QLatin1StringView("cachelog")) {
                OpenCachingCacheLogEntry logEntry = d->parseLogEntry(reader);
                logs[logEntry.cacheId()].addLogEntry(logEntry);
            }
        }
    }

    QHash<int, OpenCachingCache>::iterator itpoint = caches.begin();
    QHash<int, OpenCachingCache>::iterator const endpoint = caches.end();
    for (; itpoint != endpoint; ++itpoint) {
        if (caches[itpoint.key()].difficulty() >= m_minDifficulty && caches[itpoint.key()].difficulty() <= m_maxDifficulty) {
            caches[itpoint.key()].setDescription(descriptions[itpoint.key()]);
            caches[itpoint.key()].setLog(logs[itpoint.key()]);
            addItemToList(new OpenCachingItem(caches[itpoint.key()], this));
        }
    }
}

}

#include "moc_OpenCachingModel.cpp"
