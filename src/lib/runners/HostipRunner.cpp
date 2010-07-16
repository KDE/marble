/*
    Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
    
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "HostipRunner.h"

#include "MarbleAbstractRunner.h"
#include "MarbleDebug.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace Marble
{

HostipRunner::HostipRunner( QObject *parent ) :
        MarbleAbstractRunner( parent )
{
    // nothing to do
}

HostipRunner::~HostipRunner()
{
    quit();

    if (!wait(5000)) {
        qWarning() << "Unable to terminate HostipRunner thread properly.";
    }
}

GeoDataFeature::GeoDataVisualCategory HostipRunner::category() const
{
    return GeoDataFeature::Coordinate;
}

void HostipRunner::slotNoResults()
{
    emit runnerFinished( this, QVector<GeoDataPlacemark*>() );
}

void HostipRunner::run()
{
    if( !m_input.contains('.') ) {
        // Simple IP/hostname heuristic to avoid requests not needed:
        // String must contain at least one dot.
        slotNoResults();
    }
    else {
        // Lookup the IP address for a hostname, or the hostname if an IP address was given
        QHostInfo ::lookupHost(m_input, this, SLOT(slotLookupFinished(QHostInfo)));
    }

    exec();
}

void HostipRunner::slotLookupFinished(const QHostInfo &info)
{
    if (!info.addresses().isEmpty()) {
        m_hostInfo = info;
        QString hostAddress = info.addresses().first().toString();
        QString query = QString("http://api.hostip.info/get_html.php?ip=%1&position=true").arg(hostAddress);

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slotRequestFinished(QNetworkReply*)));
        QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(query)));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(slotNoResults()));
    }
    else
      slotNoResults();
}

void HostipRunner::slotRequestFinished( QNetworkReply* reply )
{
    double lon(0.0), lat(0.0);
    for (QString line = reply->readLine(); !line.isEmpty(); line = reply->readLine()) {
        QString lonInd = "Longitude: ";
        if (line.startsWith(lonInd)) {
            lon = line.mid(lonInd.length()).toDouble();
        }

        QString latInd = "Latitude: ";
        if (line.startsWith(latInd)) {
            lat = line.mid(latInd.length()).toDouble();
        }
    }

    QVector<GeoDataPlacemark*> placemarks;

    if (lon != 0.0 && lat != 0.0) {
        GeoDataPlacemark *placemark = new GeoDataPlacemark;

        placemark->setName(m_hostInfo.hostName());

        QString description("%1 (%2)");
        placemark->setDescription(description.
                                 arg(m_hostInfo.hostName()).
                                 arg(m_hostInfo.addresses().first().toString()));

        placemark->setCoordinate(lon * DEG2RAD, lat * DEG2RAD);
        placemark->setVisualCategory( category() );
        placemarks << placemark;
    }
    
    emit runnerFinished( this, placemarks );
}

} // namespace Marble

#include "HostipRunner.moc"
