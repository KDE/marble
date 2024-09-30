// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>

#include "HostipRunner.h"

#include "GeoDataPlacemark.h"
#include "MarbleDebug.h"

#include <QNetworkReply>
#include <QTimer>
#include <QUrl>
#include <QVector>

namespace Marble
{

HostipRunner::HostipRunner(QObject *parent)
    : SearchRunner(parent)
    , m_networkAccessManager()
{
    connect(&m_networkAccessManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(slotRequestFinished(QNetworkReply *)), Qt::DirectConnection);
}

HostipRunner::~HostipRunner()
{
}

void HostipRunner::slotNoResults()
{
    emit searchFinished(QList<GeoDataPlacemark *>());
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

        connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
        connect(this, SIGNAL(searchFinished(QList<GeoDataPlacemark *>)), &eventLoop, SLOT(quit()));

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
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotNoResults()), Qt::DirectConnection);
}

void HostipRunner::slotRequestFinished(QNetworkReply *reply)
{
    double lon(0.0), lat(0.0);
    for (QString line = reply->readLine(); !line.isEmpty(); line = reply->readLine()) {
        QString lonInd = "Longitude: ";
        if (line.startsWith(lonInd)) {
            lon = QStringView{line}.mid(lonInd.length()).toDouble();
        }

        QString latInd = "Latitude: ";
        if (line.startsWith(latInd)) {
            lat = QStringView{line}.mid(latInd.length()).toDouble();
        }
    }

    QList<GeoDataPlacemark *> placemarks;

    if (lon != 0.0 && lat != 0.0) {
        GeoDataPlacemark *placemark = new GeoDataPlacemark;

        placemark->setName(m_hostInfo.hostName());

        QString description("%1 (%2)");
        placemark->setDescription(description.arg(m_hostInfo.hostName()).arg(m_hostInfo.addresses().first().toString()));

        placemark->setCoordinate(lon * DEG2RAD, lat * DEG2RAD);
        placemark->setVisualCategory(GeoDataPlacemark::Coordinate);
        placemarks << placemark;
    }

    emit searchFinished(placemarks);
}

} // namespace Marble

#include "moc_HostipRunner.cpp"
