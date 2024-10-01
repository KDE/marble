// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>

#ifndef GEODATAMULTITRACK_H
#define GEODATAMULTITRACK_H

#include "geodata_export.h"

#include "GeoDataGeometry.h"

#include <QList>

namespace Marble
{

class GeoDataMultiTrackPrivate;
class GeoDataTrack;

/**
 */
class GEODATA_EXPORT GeoDataMultiTrack : public GeoDataGeometry
{
public:
    GeoDataMultiTrack();
    explicit GeoDataMultiTrack(const GeoDataGeometry &other);

    ~GeoDataMultiTrack() override;

    const char *nodeType() const override;

    EnumGeometryId geometryId() const override;

    GeoDataGeometry *copy() const override;

    bool operator==(const GeoDataMultiTrack &other) const;
    bool operator!=(const GeoDataMultiTrack &other) const;

    const GeoDataLatLonAltBox &latLonAltBox() const override;

    int size() const;
    GeoDataTrack &at(int pos);
    const GeoDataTrack &at(int pos) const;
    GeoDataTrack &operator[](int pos);
    const GeoDataTrack &operator[](int pos) const;

    GeoDataTrack &first();
    const GeoDataTrack &first() const;
    GeoDataTrack &last();
    const GeoDataTrack &last() const;

    /**
     * @brief  returns the requested child item
     */
    GeoDataTrack *child(int);

    /**
     * @brief  returns the requested child item
     */
    const GeoDataTrack *child(int) const;

    /**
     * @brief returns the position of an item in the list
     */
    int childPosition(const GeoDataTrack *child) const;

    /**
     * @brief add an element
     */
    void append(GeoDataTrack *other);

    GeoDataMultiTrack &operator<<(const GeoDataTrack &value);

    QList<GeoDataTrack *>::Iterator begin();
    QList<GeoDataTrack *>::Iterator end();
    QList<GeoDataTrack *>::ConstIterator constBegin() const;
    QList<GeoDataTrack *>::ConstIterator constEnd() const;
    void clear();
    QList<GeoDataTrack> vector() const;

    QList<GeoDataTrack *>::Iterator erase(QList<GeoDataTrack *>::Iterator pos);
    QList<GeoDataTrack *>::Iterator erase(QList<GeoDataTrack *>::Iterator begin, QList<GeoDataTrack *>::Iterator end);

    // Serialize the Placemark to @p stream
    void pack(QDataStream &stream) const override;
    // Unserialize the Placemark from @p stream
    void unpack(QDataStream &stream) override;

private:
    Q_DECLARE_PRIVATE(GeoDataMultiTrack)
};

}

#endif // GEODATAMULTITRACK_H
