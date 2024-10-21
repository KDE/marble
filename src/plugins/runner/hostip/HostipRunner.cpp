// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>

#include "HostipRunner.h"

#include "GeoDataPlacemark.h"
#include <QList>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>

namespace Marble
{

HostipRunner::HostipRunner(QObject *parent)
    : SearchRunner(parent)
    , m_networkAccessManager()
{
    connect(&m_networkAccessManager, &QNetworkAccessManager::finished, this, &HostipRunner::slotRequestFinished, Qt::DirectConnection);
}

HostipRunner::~HostipRunner() = default;

void HostipRunner::slotNoResults()
{
    Q_EMIT searchFinished(QList<GeoDataPlacemark *>());
}

void HostipRunner::search(const QString &searchTerm, const GeoDataLatLonBox &)
{
    if (!searchTerm.contains(QLatin1Char('.'))) {
        // Simple IP/hostname heuristic to avoid requests not needed:
        // String must contain at least one dot.
        slotNoResults();
    } else {
        QEventLoop eventLoop;

        QTimer timer;
        timer.setSingleShot(true);
        timer.setInterval(15000);

        connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
        connect(this, &SearchRunner::searchFinished, &eventLoop, &QEventLoop::quit);

        // Lookup the IP address for a hostname, or the hostname if an IP address was given
        QHostInfo ::lookupHost(searchTerm, this, SLOT(slotLookupFinished(QHostInfo)));
        timer.start();

        eventLoop.exec();
    }
}

void HostipRunner::slotLookupFinished(const QHostInfo &info)
{
    if (!info.addresses().isEmpty()) {
        m_hostInfo = info;
        QString hostAddress = info.addresses().first().toString();
        QString query = QStringLiteral("http://api.hostip.info/get_html.php?ip=%1&position=true").arg(hostAddress);
        m_request.setUrl(QUrl(query));

        // @todo FIXME Must currently be done in the main thread, see bug 257376
        QTimer::singleShot(0, this, SLOT(get()));
    } else
        slotNoResults();
}

void HostipRunner::get()
{
    QNetworkReply *reply = m_networkAccessManager.get(m_request);
    connect(reply, &QNetworkReply::errorOccurred, this, &HostipRunner::slotNoResults, Qt::DirectConnection);
}

void HostipRunner::slotRequestFinished(QNetworkReply *reply)
{
    double lon(0.0), lat(0.0);
    for (QString line = QString::fromUtf8(reply->readLine()); !line.isEmpty(); line = QString::fromUtf8(reply->readLine())) {
        QString lonInd = QStringLiteral("Longitude: ");
        if (line.startsWith(lonInd)) {
            lon = QStringView{line}.mid(lonInd.length()).toDouble();
        }

        QString latInd = QStringLiteral("Latitude: ");
        if (line.startsWith(latInd)) {
            lat = QStringView{line}.mid(latInd.length()).toDouble();
        }
    }

    QList<GeoDataPlacemark *> placemarks;

    if (lon != 0.0 && lat != 0.0) {
        auto placemark = new GeoDataPlacemark;

        placemark->setName(m_hostInfo.hostName());

        QString description(QStringLiteral("%1 (%2)"));
        placemark->setDescription(description.arg(m_hostInfo.hostName()).arg(m_hostInfo.addresses().first().toString()));

        placemark->setCoordinate(lon * DEG2RAD, lat * DEG2RAD);
        placemark->setVisualCategory(GeoDataPlacemark::Coordinate);
        placemarks << placemark;
    }

    Q_EMIT searchFinished(placemarks);
}

} // namespace Marble

#include "moc_HostipRunner.cpp"
