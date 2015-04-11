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


#include <QPixmap>

#include "GeoDataColorStyle.h"
#include "GeoDataHotSpot.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataIconStylePrivate;
class RemoteIconLoader;

// Limits for the user in case of scaling the icon too much
static const QSize s_maximumIconSize = QSize( 100, 100 );
static const QSize s_minimumIconSize = QSize( 10, 10 );

class GEODATA_EXPORT GeoDataIconStyle : public GeoDataColorStyle
{
  public:
    GeoDataIconStyle();
    GeoDataIconStyle( const GeoDataIconStyle& other );
    explicit GeoDataIconStyle( const QImage& icon,
                               const QPointF &hotSpot = QPointF( 0.5, 0.5 ) );
    ~GeoDataIconStyle();

    GeoDataIconStyle& operator=( const GeoDataIconStyle& other );

    bool operator==( const GeoDataIconStyle &other ) const;
    bool operator!=( const GeoDataIconStyle &other ) const;

    /// Provides type information for downcasting a GeoData
    virtual const char* nodeType() const;

    void setIcon( const QImage& icon );
    QImage icon() const;

    /**
     * @brief Returns a scaled version of label icon
     * @return  the scaled icon
     */
    QImage scaledIcon() const;

    void setIconPath( const QString& filename );
    QString iconPath() const;

    void setHotSpot( const QPointF& hotSpot, GeoDataHotSpot::Units xunits, GeoDataHotSpot::Units yunits );
    QPointF hotSpot( GeoDataHotSpot::Units& xunits, GeoDataHotSpot::Units& yunits ) const;

    void setScale( const float &scale );
    float scale() const;

    int heading() const;
    void setHeading( int heading );
    
    RemoteIconLoader *remoteIconLoader() const;

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
