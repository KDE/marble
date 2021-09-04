// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Torsten Rahn <rahn@kde.org>
// SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>
//


#ifndef MARBLE_GEODATAHOTSPOT_H
#define MARBLE_GEODATAHOTSPOT_H

#include <QPointF>

#include "GeoDataObject.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataHotSpotPrivate;

/**
 */
class GEODATA_EXPORT GeoDataHotSpot : public GeoDataObject
{
  public:
    enum Units{Fraction, Pixels, InsetPixels};

    GeoDataHotSpot( const QPointF& hotSpot = QPointF( 0.5, 0.5 ),
                    Units xunits = Fraction, Units yunits = Fraction );
    GeoDataHotSpot( const GeoDataHotSpot& other );
    ~GeoDataHotSpot() override;

    GeoDataHotSpot& operator=( const GeoDataHotSpot& other );
    bool operator==( const GeoDataHotSpot& other ) const;
    bool operator!=( const GeoDataHotSpot& other ) const;

    /// Provides type information for downcasting a GeoData
    const char* nodeType() const override;

    /**
    * @brief this function returns the hotspot and the units the hotspot is measured in
    * @param xunits after running hotspot, you'll receive the unit in which x is measured
    * @param yunits the same for y
    * @return the point of the hotspot
    */
    const QPointF& hotSpot( Units& xunits, Units& yunits ) const;

    void setHotSpot( const QPointF& hotSpot = QPointF( 0.5, 0.5 ),
                     Units xunits = Fraction, Units yunits = Fraction );

    /// Serialize the contents of the feature to @p stream.
    void pack( QDataStream& stream ) const override;
    /// Unserialize the contents of the feature from @p stream.
    void unpack( QDataStream& stream ) override;

  private:
    GeoDataHotSpotPrivate *const d;
};

}

#endif
