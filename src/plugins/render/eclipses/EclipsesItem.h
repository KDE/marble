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

#include <eclsolar.h>

namespace Marble
{

/**
 * @brief The representation of an eclipse event
 *
 * This class represents an eclipse event on earth. It calculates all
 * basic information like date and visibility upon initialization.
 * Expensive calculations like boundary polygons are done the first time
 * they are requested.
 *
 * The calculations are done using the eclsolar backend that has to be
 * passed to the constructor.
 */
class EclipsesItem : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief A type of an eclipse event
     */
    enum EclipsePhase {
        TotalMoon               = -4,
        PartialMoon             = -3,
        PenumbralMoon           = -1,
        PartialSun              =  1,
        NonCentralAnnularSun    =  2,
        NonCentralTotalSun      =  3,
        AnnularSun              =  4,
        TotalSun                =  5,
        AnnularTotalSun         =  6
    };

    /**
     * @brief Construct the EclipseItem object and trigger basic calculations
     * @param ecl The EclSolar backend
     * @param parent The parent object
     */
    explicit EclipsesItem( EclSolar *ecl, int index, QObject *parent = 0 );

    ~EclipsesItem();

    /**
     * @brief The index of the eclipse event
     *
     * Returns the index of the eclipse event.
     *
     * @return The eclipse events index.
     */
    int index() const;

    /**
     * @brief Check if the event takes place at a given datetime
     * @param dateTime The date time to check
     *
     * Checks whether or not this eclipse event takes place at the given
     * @p dateTime. This is true if @p dateTime is between the global
     * start and end dates of the event.
     *
     * @return True if the event takes place at @p dateTime or false otherwise.
     */
    bool takesPlaceAt( const QDateTime &dateTime ) const;

    /**
     * @brief Returns the phase of this eclipse event
     * @return the phase or type of this eclipse event
     * @see phaseText
     */
    EclipsesItem::EclipsePhase phase() const;


    /**
     * @brief Returns an icon of the eclipse type
     * @return an icon representing the eclipse's type
     */
    QIcon icon() const;

    /**
     * @brief Returns a human readable representation of the eclipse type
     * @return A string representing the eclipse's type
     * @see phase
     */
    QString phaseText() const;

    /**
     * @brief Returns the date of the eclipse event's maximum
     * @return The DateTime of the eclipse's maximum
     * @see maxLocation
     */
    const QDateTime& dateMaximum() const;

    /**
     * @brief Returns the start date of the eclipse's partial phase
     * @return The start date of the partial phase
     * @see endDatePartial
     */
    const QDateTime& startDatePartial() const;

    /**
     * @brief Returns the end date of the eclipse's partial phase
     * @return The end date of the partial phase
     * @see startDatePartial
     */
    const QDateTime& endDatePartial() const;

    /**
     * @brief Returns the number of hours the partial phase takes place
     * @return The number of hours of the partial phase
     * @see startDatePartial, endDatePartial
     */
    int partialDurationHours() const;

    /**
     * @brief Returns the start date of the eclipse's total phase
     *
     * If the eclipse has a total phase, the date and time of its beginning
     * is returned. If this is no total eclipse, an invalid datetime object
     * will be returned.
     *
     * @return The start date of the total phase or an invalid date
     * @see endDateTotal
     */
    const QDateTime& startDateTotal() const;

    /**
     * @brief Returns the end date of the eclipse's total phase
     *
     * If the eclipse has a total phase, the date and time of its ending
     * is returned. If this is no total eclipse, an invalid datetime object
     * will be returned.
     *
     * @return The end date of the total phase or an invalid date
     * @see startDateTotal
     */
    const QDateTime& endDateTotal() const;

    /**
     * @brief Return the eclipse's magnitude
     * @return The magnitude of the eclipse
     */
    double magnitude() const;

    /**
     * @brief Return the coordinates of the eclipse's maximum
     * @return GeoDataCoordinates of the eclipse's maximum
     * @see dateMaximum
     */
    const GeoDataCoordinates& maxLocation();

    /**
     * @brief The eclipse's central line
     * @return The central line of the eclipse
     */
    const GeoDataLineString& centralLine();

    /**
     * @brief Return the eclipse's umbra
     * @return The eclipse's umbra
     */
    const GeoDataLinearRing& umbra();

    /**
     * @brief Return the eclipse's southern penumbra
     * @return The eclipse's southern penumbra
     */
    const GeoDataLineString& southernPenumbra();

    /**
     * @brief Return the eclipse's northern penumbra
     * @return The eclipse's northern umbra
     */
    const GeoDataLineString& northernPenumbra();

    /**
     * @brief Return the eclipse's sun boundaries
     * @return The eclipse's sun boundaries
     */
    const QList<GeoDataLinearRing>& sunBoundaries();

    /**
     * @brief Return the shadow cone of the umbra
     * @return The shadow cone of the umbra
     */
    GeoDataLinearRing shadowConeUmbra();

    /**
     * @brief Return the shadow cone of the penumbra
     * @return The shadow cone of the penumbra
     */
    GeoDataLinearRing shadowConePenumbra();

    /**
     * @brief Return the shadow cone of the penumbra at 60 percent magnitude
     * @return The shadow cone of the penumbra at 60 percent magnitude
     */
    GeoDataLinearRing shadowCone60MagPenumbra();

private:
    /**
     * @brief Initialize the eclipse item
     *
     * Initializes all properties of the eclipse item and does basic
     * calculations. Expensive calculations are done as required.
     *
     * @see calculate
     */
    void initialize();

    /**
     * @brief Do detailed calculations
     *
     * Do the expensive calculations (like shadow cones) for this eclipse
     * event. This is normally called on the first request of such data.
     */
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
    GeoDataLineString m_southernPenumbra;
    GeoDataLineString m_northernPenumbra;
    GeoDataLinearRing m_shadowConeUmbra;
    GeoDataLinearRing m_shadowConePenumbra;
    GeoDataLinearRing m_shadowCone60MagPenumbra;
    QList<GeoDataLinearRing> m_sunBoundaries;
};

}

#endif // MARBLE_ECLIPSESITEM_H

