// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef GEODATALISTSTYLE_H
#define GEODATALISTSTYLE_H

#include "GeoDataObject.h"
#include "MarbleGlobal.h"
#include "geodata_export.h"

#include <QColor>
#include <QList>

namespace Marble
{

class GeoDataListStylePrivate;
class GeoDataItemIcon;

/**
 */
class GEODATA_EXPORT GeoDataListStyle : public GeoDataObject
{
public:
    GeoDataListStyle();

    GeoDataListStyle(const GeoDataListStyle &other);

    GeoDataListStyle &operator=(const GeoDataListStyle &other);

    bool operator==(const GeoDataListStyle &other) const;
    bool operator!=(const GeoDataListStyle &other) const;

    ~GeoDataListStyle() override;

    /** Provides type information for downcasting a GeoNode */
    const char *nodeType() const override;

    enum ListItemType {
        Check,
        RadioFolder,
        CheckOffOnly,
        CheckHideChildren
    };

    ListItemType listItemType() const;
    void setListItemType(ListItemType type);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

    QList<GeoDataItemIcon *> itemIconList() const;

    GeoDataItemIcon *child(int);

    const GeoDataItemIcon *child(int) const;

    int childPosition(const GeoDataItemIcon *child) const;

    void append(GeoDataItemIcon *other);

    void remove(int index);

    int size() const;

    GeoDataItemIcon &at(int pos);
    const GeoDataItemIcon &at(int pos) const;

    GeoDataItemIcon &last();
    const GeoDataItemIcon &last() const;

    GeoDataItemIcon &first();
    const GeoDataItemIcon &first() const;

    QList<GeoDataItemIcon *>::Iterator begin();
    QList<GeoDataItemIcon *>::Iterator end();
    QList<GeoDataItemIcon *>::ConstIterator constBegin() const;
    QList<GeoDataItemIcon *>::ConstIterator constEnd() const;
    void clear();

    void pack(QDataStream &stream) const override;

    void unpack(QDataStream &stream) override;

private:
    friend class GeoDataItemIcon;
    GeoDataListStylePrivate *const d;
};

}

#endif
