// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Niko Sams <niko.sams@gmail.com>
//

#ifndef MARBLE_GEODATASIMPLEARRAYDATA_H
#define MARBLE_GEODATASIMPLEARRAYDATA_H

#include "GeoDataObject.h"

class QVariant;

namespace Marble
{

class GeoDataSimpleArrayDataPrivate;

/**
 */
class GEODATA_EXPORT GeoDataSimpleArrayData : public GeoDataObject
{
public:
    GeoDataSimpleArrayData();
    GeoDataSimpleArrayData(const GeoDataSimpleArrayData &other);
    bool operator==(const GeoDataSimpleArrayData &other) const;
    bool operator!=(const GeoDataSimpleArrayData &other) const;
    ~GeoDataSimpleArrayData() override;

    /**
     * Returns the number of value in the array
     */
    int size() const;

    /**
     * Returns the value at index @p index
     */
    QVariant valueAt(int index) const;

    /**
     * Returns all values in the array
     */
    QList<QVariant> valuesList() const;

    /**
     * Append a value to the array
     */
    void append(const QVariant &value);

    const char *nodeType() const override;
    void pack(QDataStream &stream) const override;
    void unpack(QDataStream &stream) override;

private:
    GeoDataSimpleArrayDataPrivate *const d;
};

}

#endif // MARBLE_GEODATASIMPLEARRAYDATA_H
