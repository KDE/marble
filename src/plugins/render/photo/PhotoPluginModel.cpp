// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

// Self
#include "PhotoPluginModel.h"

// Photo Plugin
#include "FlickrParser.h"
#include "PhotoPlugin.h"
#include "PhotoPluginItem.h"

// Marble
#include "AbstractDataPluginItem.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"

// Qt
#include <QHash>
#include <QString>
#include <QUrl>

using namespace Marble;

const QString flickrApiKey(QStringLiteral("620131a1b82b000c9582b94effcdc636"));

PhotoPluginModel::PhotoPluginModel(const MarbleModel *marbleModel, QObject *parent)
    : AbstractDataPluginModel(QStringLiteral("photo"), marbleModel, parent)
    , m_marbleWidget(nullptr)
{
}

QUrl PhotoPluginModel::generateUrl(const QString &service, const QString &method, const QHash<QString, QString> &options)
{
    QString url;

    if (service == QLatin1StringView("flickr"))
        url += QLatin1StringView("https://www.flickr.com/services/rest/");
    else
        return {};

    url += QLatin1StringView("?method=") + method + QLatin1StringView("&format=rest") + QLatin1StringView("&api_key=") + flickrApiKey;

    QHash<QString, QString>::const_iterator it = options.constBegin();
    QHash<QString, QString>::const_iterator const end = options.constEnd();
    for (; it != end; ++it) {
        url += QLatin1Char('&') + it.key() + QLatin1Char('=') + it.value();
    }

    return QUrl(url);
}

void PhotoPluginModel::getAdditionalItems(const GeoDataLatLonAltBox &box, qint32 number)
{
    // Flickr only supports images for earth
    if (marbleModel()->planetId() != QLatin1StringView("earth")) {
        return;
    }

    if (box.west() <= box.east()) {
        const QString bbox = QString::number(box.west() * RAD2DEG) + QLatin1Char(',') + QString::number(box.south() * RAD2DEG) + QLatin1Char(',')
            + QString::number(box.east() * RAD2DEG) + QLatin1Char(',') + QString::number(box.north() * RAD2DEG);

        QHash<QString, QString> options;
        options.insert(QStringLiteral("per_page"), QString::number(number));
        options.insert(QStringLiteral("bbox"), bbox);
        options.insert(QStringLiteral("sort"), QStringLiteral("interestingness-desc"));
        options.insert(QStringLiteral("license"), m_licenses);

        downloadDescriptionFile(generateUrl(QStringLiteral("flickr"), QStringLiteral("flickr.photos.search"), options));
    } else {
        // Flickr api doesn't support bboxes with west > east so we have to split in two boxes
        const QString bboxWest = QString::number(box.west() * RAD2DEG) + QLatin1Char(',') + QString::number(box.south() * RAD2DEG) + QLatin1Char(',')
            + QString::number(180) + QLatin1Char(',') + QString::number(box.north() * RAD2DEG);

        QHash<QString, QString> optionsWest;
        optionsWest.insert(QStringLiteral("per_page"), QString::number(number / 2));
        optionsWest.insert(QStringLiteral("bbox"), bboxWest);
        optionsWest.insert(QStringLiteral("sort"), QStringLiteral("interestingness-desc"));
        optionsWest.insert(QStringLiteral("license"), m_licenses);

        downloadDescriptionFile(generateUrl(QStringLiteral("flickr"), QStringLiteral("flickr.photos.search"), optionsWest));

        const QString bboxEast = QString::number(-180) + QLatin1Char(',') + QString::number(box.south() * RAD2DEG) + QLatin1Char(',')
            + QString::number(box.east() * RAD2DEG) + QLatin1Char(',') + QString::number(box.north() * RAD2DEG);

        QHash<QString, QString> optionsEast;
        optionsEast.insert(QStringLiteral("per_page"), QString::number(number / 2));
        optionsEast.insert(QStringLiteral("bbox"), bboxEast);
        optionsEast.insert(QStringLiteral("sort"), QStringLiteral("interestingness-desc"));
        optionsEast.insert(QStringLiteral("license"), m_licenses);

        downloadDescriptionFile(generateUrl(QStringLiteral("flickr"), QStringLiteral("flickr.photos.search"), optionsEast));
    }
}

void PhotoPluginModel::parseFile(const QByteArray &file)
{
    QList<PhotoPluginItem *> list;
    FlickrParser parser(m_marbleWidget, &list, this);

    parser.read(file);

    QList<PhotoPluginItem *>::iterator it;
    QList<AbstractDataPluginItem *> items;

    for (it = list.begin(); it != list.end(); ++it) {
        if (itemExists((*it)->id())) {
            delete (*it);
            continue;
        }

        downloadItem((*it)->photoUrl(), QStringLiteral("thumbnail"), (*it));
        downloadItem((*it)->infoUrl(), QStringLiteral("info"), (*it));
        items << *it;
    }
    addItemsToList(items);
}

void PhotoPluginModel::setMarbleWidget(MarbleWidget *widget)
{
    m_marbleWidget = widget;
}

void PhotoPluginModel::setLicenseValues(const QString &licenses)
{
    m_licenses = licenses;
}

#include "moc_PhotoPluginModel.cpp"
