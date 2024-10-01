// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAMULTIGEOMETRY_H
#define MARBLE_GEODATAMULTIGEOMETRY_H

#include "geodata_export.h"

#include "GeoDataGeometry.h"
#include <QList>

namespace Marble
{

class GeoDataMultiGeometryPrivate;

/**
 * @short A class that can contain other GeoDataGeometry objects
 *
 * GeoDataMultiGeometry is a collection of other GeoDataGeometry objects.
 * As one can add GeoDataMultiGeometry to itself, you can make up a collection
 * of different objects to form one Placemark.
 */
class GEODATA_EXPORT GeoDataMultiGeometry : public GeoDataGeometry
{
public:
    GeoDataMultiGeometry();
    explicit GeoDataMultiGeometry(const GeoDataGeometry &other);

    ~GeoDataMultiGeometry() override;

    const char *nodeType() const override;

    EnumGeometryId geometryId() const override;

    GeoDataGeometry *copy() const override;

    bool operator==(const GeoDataMultiGeometry &other) const;
    bool operator!=(const GeoDataMultiGeometry &other) const
    {
        return !(*this == other);
    }

    const GeoDataLatLonAltBox &latLonAltBox() const override;

    int size() const;
    GeoDataGeometry &at(int pos);
    const GeoDataGeometry &at(int pos) const;
    GeoDataGeometry &operator[](int pos);
    const GeoDataGeometry &operator[](int pos) const;

    GeoDataGeometry &first();
    const GeoDataGeometry &first() const;
    GeoDataGeometry &last();
    const GeoDataGeometry &last() const;

    /**
     * @brief  returns the requested child item
     */
    GeoDataGeometry *child(int);

    /**
     * @brief  returns the requested child item
     */
    const GeoDataGeometry *child(int) const;

    /**
     * @brief returns the position of an item in the list
     */
    int childPosition(const GeoDataGeometry *child) const;

    /**
     * @brief add an element
     */
    void append(GeoDataGeometry *other);

    GeoDataMultiGeometry &operator<<(const GeoDataGeometry &value);

    QList<GeoDataGeometry *>::Iterator begin();
    QList<GeoDataGeometry *>::Iterator end();
    QList<GeoDataGeometry *>::ConstIterator constBegin() const;
    QList<GeoDataGeometry *>::ConstIterator constEnd() const;
    void clear();
    QList<GeoDataGeometry *> vector();

    QList<GeoDataGeometry *>::Iterator erase(QList<GeoDataGeometry *>::Iterator pos);
    QList<GeoDataGeometry *>::Iterator erase(QList<GeoDataGeometry *>::Iterator begin, QList<GeoDataGeometry *>::Iterator end);

    // Serialize the Placemark to @p stream
    void pack(QDataStream &stream) const override;
    // Unserialize the Placemark from @p stream
    void unpack(QDataStream &stream) override;

private:
    Q_DECLARE_PRIVATE(GeoDataMultiGeometry)
};

}

#endif
