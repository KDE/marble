//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef GEODATAITEMICON_H
#define GEODATAITEMICON_H

#include <QFlags>

#include "GeoDataObject.h"

class QImage;

namespace Marble
{

class GeoDataItemIconPrivate;

/**
 */
class GEODATA_EXPORT GeoDataItemIcon : public GeoDataObject
{
    Q_FLAGS( ItemIconState ItemIconStates )
public:
    GeoDataItemIcon();

    GeoDataItemIcon( const GeoDataItemIcon &other );

    GeoDataItemIcon& operator=( const GeoDataItemIcon &other );
    bool operator==( const GeoDataItemIcon &other ) const;
    bool operator!=( const GeoDataItemIcon &other ) const;

    ~GeoDataItemIcon() override;

    /** Provides type information for downcasting a GeoNode */
    const char* nodeType() const override;

    enum ItemIconState {
        Open = 1 << 0,
        Closed = 1 << 1,
        Error = 1 << 2,
        Fetching0 = 1 << 3,
        Fetching1 = 1 << 4,
        Fetching2 = 1 << 5
    };
    Q_DECLARE_FLAGS( ItemIconStates, ItemIconState )

    ItemIconStates state() const;
    void setState(ItemIconStates state);

    QString iconPath() const;
    void setIconPath( const QString &path );

    QImage icon() const;
    void setIcon( const QImage &icon );

private:
    GeoDataItemIconPrivate* const d;
};

}

#endif
