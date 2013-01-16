//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_ECLIPSESITEM_H
#define MARBLE_ECLIPSESITEM_H

#include <QObject>
#include <QDateTime>

#include "GeoDataLineString.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLinearRing.h"

#include "ecl/eclsolar.h"

namespace Marble
{

class EclipsesItem : public QObject
{
    Q_OBJECT
public:

    enum EclipsePhase {
        PartialSun              = 1,
        NonCentralAnnularSun    = 2,
        NonCentralTotalSun      = 3,
        AnnularSun              = 4,
        TotalSun                = 5,
        AnnularTotalSun         = 6
    };

    EclipsesItem( EclSolar *ecl, int index, QObject *parent = 0 );
    ~EclipsesItem();

    int index() const;

    bool takesPlaceAt( const QDateTime &dateTime ) const;

    QDateTime dateTime() const;

    EclipsesItem::EclipsePhase phase() const;
    QString phaseText() const;

    const QDateTime& dateMaximum() const;
    const QDateTime& startDatePartial() const;
    const QDateTime& endDatePartial() const;
    int partialDurationHours() const;
    const QDateTime& startDateTotal() const;
    const QDateTime& endDateTotal() const;

    double magnitude() const;

    const GeoDataCoordinates& maxLocation();

    const GeoDataLineString& centralLine();

    const GeoDataLinearRing& umbra();
    const GeoDataLineString& southernPenUmbra();
    const GeoDataLineString& northernPenUmbra();

    const QList<GeoDataLinearRing>& sunBoundaries();

    const QList<GeoDataCoordinates>& shadowConeUmbra();
    const QList<GeoDataCoordinates>& shadowConePenUmbra();
    const QList<GeoDataCoordinates>& shadowCone60MagPenUmbra();

private:
    void initialize();
    void calculate();

    EclSolar *m_ecl;
    int m_index;
    bool m_calculationsNeedUpdate;
    bool m_isTotal;
    QDateTime m_dateMaximum;
    QDateTime m_startDatePartial;
    QDateTime m_endDatePartial;
    QDateTime m_startDateTotal;
    QDateTime m_endDateTotal;
    EclipsesItem::EclipsePhase m_phase;
    double m_magnitude;

    GeoDataCoordinates m_maxLocation;
    GeoDataLineString m_centralLine;
    GeoDataLinearRing m_umbra;
    GeoDataLineString m_southernPenUmbra;
    GeoDataLineString m_northernPenUmbra;
    QList<GeoDataCoordinates> m_shadowConeUmbra;
    QList<GeoDataCoordinates> m_shadowConePenUmbra;
    QList<GeoDataCoordinates> m_shadowCone60MagPenUmbra;
    QList<GeoDataLinearRing> m_sunBoundaries;
};

}

#endif // MARBLE_ECLIPSESITEM_H

