//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
//


#ifndef MARBLE_GEODATAICONSTYLE_H
#define MARBLE_GEODATAICONSTYLE_H


#include <QtGui/QPixmap>

#include "GeoDataColorStyle.h"
#include "GeoDataHotSpot.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataIconStylePrivate;

class GEODATA_EXPORT GeoDataIconStyle : public GeoDataColorStyle
{
  public:
    GeoDataIconStyle();
    GeoDataIconStyle( const GeoDataIconStyle& other );
    explicit GeoDataIconStyle( const QPixmap& icon, 
                               const QPointF &hotSpot = QPointF( 0.5, 0.5 ) );
    ~GeoDataIconStyle();

    GeoDataIconStyle& operator=( const GeoDataIconStyle& other );

    /// Provides type information for downcasting a GeoData
    virtual QString nodeType() const;

    void setIcon( const QPixmap& icon );
    QPixmap icon() const;

    void setIconPath( const QString& filename );

    void setHotSpot( const QPointF& hotSpot, GeoDataHotSpot::Units xunits, GeoDataHotSpot::Units yunits );
    const QPointF& hotSpot() const;

    void setScale( const float &scale );
    float scale() const;

    /*
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

  private:
    GeoDataIconStylePrivate * const d;
};

}

#endif
