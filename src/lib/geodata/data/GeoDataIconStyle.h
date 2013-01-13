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
    explicit GeoDataIconStyle( const QImage& icon,
                               const QPointF &hotSpot = QPointF( 0.5, 0.5 ) );
    ~GeoDataIconStyle();

    GeoDataIconStyle& operator=( const GeoDataIconStyle& other );

    /// Provides type information for downcasting a GeoData
    virtual const char* nodeType() const;

    void setIcon( const QImage& icon );
    QImage icon() const;

    void setIconPath( const QString& filename );
    QString iconPath() const;

    void setHotSpot( const QPointF& hotSpot, GeoDataHotSpot::Units xunits, GeoDataHotSpot::Units yunits );
    QPointF hotSpot( GeoDataHotSpot::Units& xunits, GeoDataHotSpot::Units& yunits ) const;

    /** Convenience method that transforms the hotspot to the topleft corner */
    const QPointF& hotSpot() const;

    void setScale( const float &scale );
    float scale() const;

    int heading() const;
    void setHeading( int heading );

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
