//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_SATELLITESMSCITEM_H
#define MARBLE_SATELLITESMSCITEM_H

#include "TrackerPluginItem.h"

#include <QtCore/QString>
#include <QtCore/QDateTime>

#include "mex/planetarySats.h"

class QColor;

namespace Marble {

class GeoDataTrack;
class MarbleClock;
class GeoDataPlacemark;

/**
 * An instance of SatellitesMSCItem represents an item of a Marble
 * satellites catalog.
 */
class SatellitesMSCItem : public TrackerPluginItem
{
    Q_OBJECT

public:
    SatellitesMSCItem( const QString &name,
                       const QString &category,
                       const QString &relatedBody,
                       const QString &catalog,
                       const QDateTime &missionStart,
                       const QDateTime &missionEnd,
                       int catalogIndex,
                       PlanetarySats *planSat,
                       const MarbleClock *clock );
    ~SatellitesMSCItem();

    QString name() const;
    QString category() const;
    QString relatedBody() const;
    QString catalog() const;
    int catalogIndex() const;
    QString id() const;

    const QDateTime& missionStart() const;
    const QDateTime& missionEnd() const;

    void update();
    void showOrbit( bool show );
    void setOrbitColor( const QColor &color );

private:
    bool m_showOrbit;
    GeoDataTrack *m_track;
    const MarbleClock *m_clock;
    PlanetarySats *m_planSat;
    const QString m_name;
    const QString m_category;
    const QString m_relatedBody;
    const QString m_catalog;
    const int m_catalogIndex;

    double m_perc;
    double m_apoc;
    double m_inc;
    double m_ecc;
    double m_ra;
    double m_tano;
    double m_m0;
    double m_a;
    double m_n0;

    const QDateTime m_missionStart;
    const QDateTime m_missionEnd;

    void setDescription();
    void addTrackPointAt( const QDateTime &dateTime );
};

} // namespace Marble

#endif // MARBLE_SATELLITESMSCITEM_H
