// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_GEODATATRACK_H
#define MARBLE_GEODATATRACK_H

#include "GeoDataGeometry.h"

#include <QList>

class QDateTime;

namespace Marble {

class GeoDataTrackPrivate;
class GeoDataExtendedData;
class GeoDataLineString;
class GeoDataCoordinates;

/**
 * @class GeoDataTrack
 * @brief A geometry for tracking objects made of (time, coordinates) pairs
 *
 * GeoDataTrack implements the Track tag defined in Google's extension of the
 * Open Geospatial Consortium standard KML 2.2 at
 * https://developers.google.com/kml/documentation/kmlreference#gxtrack .
 *
 * A track is made of points, each point has a coordinates and a time value
 * associated with the coordinates. New points can be added using the addPoint()
 * method. The coordinates of the tracked object at a particular time can be
 * found using coordinatesAt(), you can specify if interpolation should be used
 * using the setInterpolate() function.
 *
 * By default, a LineString that passes through every coordinates in the track
 * is drawn. You can customize it by changing the GeoDataLineStyle, for example
 * if the GeoDataTrack is the geometry of feature, you can disable the line drawing with:
 * @code
 * feature->style()->lineStyle().setPenStyle( Qt::NoPen );
 * @endcode
 *
 * For convenience, the methods appendCoordinates() and appendWhen() are provided.
 * They let you add points by specifying their coordinates and time value separately.
 * When N calls to one of these methods are followed by N calls to the other,
 * the first coordinates will be matched with the first time value, the second
 * coordinates with the second time value, etc. This follows the way "coord"
 * and "when" tags inside the Track tag should be parsed.
 */
class GEODATA_EXPORT GeoDataTrack : public GeoDataGeometry
{

public:
    GeoDataTrack();
    explicit GeoDataTrack( const GeoDataTrack &other );

    GeoDataTrack &operator=( const GeoDataTrack &other );

    const char *nodeType() const override;

    EnumGeometryId geometryId() const override;

    GeoDataGeometry *copy() const override;

    /**
     * Returns the number of points in the track
     */
    int size() const;

    /**
     * @brief: Equality operators.
     */
    bool operator==( const GeoDataTrack& other ) const;
    bool operator!=( const GeoDataTrack& other ) const;

    /**
     * Returns true if coordinatesAt() should use interpolation, false otherwise.
     * The default is false.
     *
     * @see setInterpolate, coordinatesAt
     */
    bool interpolate() const;

    /**
     * Set whether coordinatesAt() should use interpolation.
     *
     * @see interpolate, coordinatesAt
     */
    void setInterpolate(bool on);

    /**
     * Return the time value of the first point in the track, or
     * an invalid QDateTime if the track is empty.
     */
    QDateTime firstWhen() const;

    /**
     * Return the time value of the last point in the track, or
     * an invalid QDateTime if the track is empty.
     */
    QDateTime lastWhen() const;

    /**
     * Returns the coordinates of all the points in the map, sorted by their
     * time value
     */
    QVector<GeoDataCoordinates> coordinatesList() const;

    /**
     * Returns the time value of all the points in the map, in chronological
     * order.
     * @since 0.26.0
     */
    QVector<QDateTime> whenList() const;

    /**
     * If interpolate() is true, return the coordinates interpolated from the
     * time values before and after @p when, otherwise return the coordinates
     * of the point with the closest time value less than or equal to @p when.
     *
     * @see interpolate
     */
    GeoDataCoordinates coordinatesAt( const QDateTime &when ) const;

    /**
     * Return coordinates at specified index. This is useful when the track contains
     * coordinates without time information.
     */
    GeoDataCoordinates coordinatesAt( int index ) const;

    /**
     * Add a new point with coordinates @p coord associated with the
     * time value @p when
     */
    void addPoint( const QDateTime &when, const GeoDataCoordinates &coord );

    /**
     * Add the coordinates part for a new point. See this class description
     * for more information.
     * @see appendWhen
     */
    void appendCoordinates( const GeoDataCoordinates &coord );

    /**
     * Add altitude information to the last appended coordinates
     */
    void appendAltitude( qreal altitude );

    /**
     * Add the time value part for a new point. See this class description
     * for more information.
     * @see appendCoordinates
     */
    void appendWhen( const QDateTime &when );

    /**
     * Remove all the points contained in the track.
     */
    void clear();

    /**
     * Remove all points from the track whose time value is less than @p when.
     */
    void removeBefore( const QDateTime &when );

    /**
     * Remove all points from the track whose time value is greater than @p when.
     */
    void removeAfter( const QDateTime &when );

    /**
     * Return the GeoDataLineString representing the current track
     */
    const GeoDataLineString *lineString() const;

    /**
     * Return the ExtendedData assigned to the feature.
     */
    const GeoDataExtendedData& extendedData() const;
    GeoDataExtendedData& extendedData();

    /**
     * Sets the ExtendedData of the feature.
     * @param  extendedData  the new ExtendedData to be used.
     */
    void setExtendedData( const GeoDataExtendedData& extendedData );

    const GeoDataLatLonAltBox& latLonAltBox() const override;
    void pack( QDataStream& stream ) const override;
    void unpack( QDataStream& stream ) override;

private:
    Q_DECLARE_PRIVATE(GeoDataTrack)
};

}

Q_DECLARE_METATYPE( Marble::GeoDataTrack* )

#endif // MARBLE_GEODATATRACK_H
