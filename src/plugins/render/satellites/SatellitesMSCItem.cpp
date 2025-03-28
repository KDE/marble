// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "SatellitesMSCItem.h"

#include <QColor>
#include <QFile>

#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTrack.h"
#include "GeoPainter.h"
#include "MarbleClock.h"

namespace Marble
{

SatellitesMSCItem::SatellitesMSCItem(const QString &name,
                                     const QString &category,
                                     const QString &relatedBody,
                                     const QString &catalog,
                                     const QDateTime &missionStart,
                                     const QDateTime &missionEnd,
                                     int catalogIndex,
                                     PlanetarySats *planSat,
                                     const MarbleClock *clock)
    : TrackerPluginItem(name)
    , m_track(new GeoDataTrack())
    , m_clock(clock)
    , m_planSat(planSat)
    , m_category(category)
    , m_relatedBody(relatedBody)
    , m_catalog(catalog)
    , m_catalogIndex(catalogIndex)
    , m_missionStart(missionStart)
    , m_missionEnd(missionEnd)
{
    placemark()->setVisualCategory(GeoDataPlacemark::Satellite);
    placemark()->setZoomLevel(0);
    placemark()->setGeometry(m_track);

    m_planSat->getKeplerElements(m_perc, m_apoc, m_inc, m_ecc, m_ra, m_tano, m_m0, m_a, m_n0);

    m_period = 86400. / m_n0;
    m_step_secs = m_period / 500;

    setDescription();
    update();
}

SatellitesMSCItem::~SatellitesMSCItem()
{
    delete m_planSat;
}

QString SatellitesMSCItem::category() const
{
    return m_category;
}

QString SatellitesMSCItem::relatedBody() const
{
    return m_relatedBody;
}

QString SatellitesMSCItem::catalog() const
{
    return m_catalog;
}

int SatellitesMSCItem::catalogIndex() const
{
    return m_catalogIndex;
}

QString SatellitesMSCItem::id() const
{
    return QStringLiteral("%1:%2").arg(catalog()).arg(catalogIndex());
}

const QDateTime &SatellitesMSCItem::missionStart() const
{
    return m_missionStart;
}

const QDateTime &SatellitesMSCItem::missionEnd() const
{
    return m_missionEnd;
}

void SatellitesMSCItem::setDescription()
{
    /*QString description =
      QObject::tr( "Object name: %1 <br />"
                   "Category: %2 <br />"
                   "Pericentre: %3 km<br />"
                   "Apocentre: %4 km<br />"
                   "Inclination: %5 Degree<br />"
                   "Revolutions per day (24h): %6" )
        .arg( name(), category(), QString::number( m_perc, 'f', 2 ),
                                  QString::number( m_apoc, 'f', 2 ),
                                  QString::number( m_inc, 'f', 2 ),
                                  QString::number( m_n0, 'f', 2 ) );*/

    QFile templateFile(QStringLiteral(":/marble/satellites/satellite.html"));
    if (!templateFile.open(QIODevice::ReadOnly)) {
        placemark()->setDescription(QObject::tr("No info available."));
        return;
    }
    QString html = QString::fromLocal8Bit(templateFile.readAll());

    html.replace(QStringLiteral("%name%"), name());
    html.replace(QStringLiteral("%noradId%"), QString::number(catalogIndex()));
    html.replace(QStringLiteral("%perigee%"), QString::number(m_perc, 'f', 2));
    html.replace(QStringLiteral("%apogee%"), QString::number(m_apoc, 'f', 2));
    html.replace(QStringLiteral("%inclination%"), QString::number(m_inc, 'f', 2));
    html.replace(QStringLiteral("%period%"), QStringLiteral("?"));
    html.replace(QStringLiteral("%semiMajorAxis%"), QStringLiteral("?"));

    placemark()->setDescription(html);
}

void SatellitesMSCItem::update()
{
    if (m_missionStart.isValid()) {
        setVisible((m_clock->dateTime() > m_missionStart));
    }

    if (m_missionEnd.isValid()) {
        setVisible((m_clock->dateTime() < m_missionEnd));
    }

    if (!isEnabled() || !isVisible()) {
        return;
    }

    QDateTime t = m_clock->dateTime();
    QDateTime endTime = t;
    if (isTrackVisible()) {
        t = t.addSecs(-m_period / 2.);
        endTime = t.addSecs(m_period);

        m_track->removeBefore(t);
        m_track->removeAfter(endTime);

        if (m_track->firstWhen().isValid() && m_track->firstWhen() < t) {
            t = m_track->firstWhen().addSecs(m_step_secs);
        }

        for (; t < endTime; t = t.addSecs(m_step_secs)) {
            addTrackPointAt(t);
        }
    } else {
        m_track->clear();
    }

    addTrackPointAt(m_clock->dateTime());
}

void SatellitesMSCItem::addTrackPointAt(const QDateTime &dateTime)
{
    double lng = 0.;
    double lat = 0.;
    double height = 0.;

    QDateTime dt = dateTime.toUTC();
    QDate date = dt.date();
    QTime time = dt.time();

    m_planSat->setMJD(date.year(), date.month(), date.day(), time.hour(), time.minute(), time.second());
    m_planSat->currentPos();
    m_planSat->getPlanetographic(lng, lat, height);

    m_track->addPoint(dateTime, GeoDataCoordinates(lng, lat, height * 1000, GeoDataCoordinates::Degree));
}

} // namespace Marble
