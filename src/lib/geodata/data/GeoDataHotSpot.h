//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <rahn@kde.org>
// Copyright 2008      Inge Wallin <inge@lysator.liu.se>
//


#ifndef MARBLE_GEODATAHOTSPOT_H
#define MARBLE_GEODATAHOTSPOT_H

#include <QtCore/QPoint>

#include "GeoDataObject.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataHotSpotPrivate;

class GEODATA_EXPORT GeoDataHotSpot : public GeoDataObject
{
  public:
    enum Units{Fraction, Pixels, InsetPixels};

    GeoDataHotSpot( const QPointF& hotSpot = QPointF( 0.5, 0.5 ),
                    Units xunits = Fraction, Units yunits = Fraction );
    GeoDataHotSpot( const GeoDataHotSpot& other );
    ~GeoDataHotSpot();

    GeoDataHotSpot& operator=( const GeoDataHotSpot& other );

    /// Provides type information for downcasting a GeoData
    virtual QString nodeType() const;

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
    virtual void pack( QDataStream& stream ) const;
    /// Unserialize the contents of the feature from @p stream.
    virtual void unpack( QDataStream& stream );

  private:
    GeoDataHotSpotPrivate *const d;
};

}

#endif
