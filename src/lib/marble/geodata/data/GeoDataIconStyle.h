// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Murad Tagirov <tmurad@gmail.com>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//


#ifndef MARBLE_GEODATAICONSTYLE_H
#define MARBLE_GEODATAICONSTYLE_H


#include "GeoDataColorStyle.h"
#include "GeoDataHotSpot.h"

#include "geodata_export.h"

#include <QSize>

namespace Marble
{

class GeoDataIconStylePrivate;
class RemoteIconLoader;

// Limits for the user in case of scaling the icon too much
static const QSize s_maximumIconSize = QSize( 100, 100 );
static const QSize s_minimumIconSize = QSize( 10, 10 );

/**
 */
class GEODATA_EXPORT GeoDataIconStyle : public GeoDataColorStyle
{
  public:
    GeoDataIconStyle();
    GeoDataIconStyle( const GeoDataIconStyle& other );
    explicit GeoDataIconStyle( const QString& iconPath,
                               const QPointF &hotSpot = QPointF( 0.5, 0.5 ) );
    ~GeoDataIconStyle() override;

    GeoDataIconStyle& operator=( const GeoDataIconStyle& other );

    bool operator==( const GeoDataIconStyle &other ) const;
    bool operator!=( const GeoDataIconStyle &other ) const;

    /// Provides type information for downcasting a GeoData
    const char* nodeType() const override;

    void setIcon( const QImage& icon );
    QImage icon() const;

    /**
     * @brief Returns a scaled version of label icon
     * @return  the scaled icon
     */
    QImage scaledIcon() const;

    /**
     * @brief setIconPath Set the path to load the icon from. Any existing icon is invalidated.
     * @param filename Path to the icon to load. Can also be a virtual file system like qrc:/
     */
    void setIconPath( const QString& filename );
    QString iconPath() const;

    void setHotSpot( const QPointF& hotSpot, GeoDataHotSpot::Units xunits, GeoDataHotSpot::Units yunits );
    QPointF hotSpot( GeoDataHotSpot::Units& xunits, GeoDataHotSpot::Units& yunits ) const;

    /**
     * @brief setSize Change the size of @see icon(). A null size (0, 0) is treated as a request to keep
     * the original icon size. Otherwise the icon will be scaled to the given size. This is especially useful
     * for vector graphics like .svg or to keep the memory footprint low when loading large images.
     * @param size Size in pixel that @see icon() should have. When the icon was previously set by #setIcon,
     * it is resized unless a null size is passed. Otherwise icon() will be scaled to the given size.
     * @param aspectRatioMode Aspect ratio mode.
     */
    void setSize(const QSize &size, Qt::AspectRatioMode aspectRatioMode=Qt::KeepAspectRatio);

    /**
     * @brief size Returns the size @see icon() will have if it can be loaded from @see iconPath.
     * A null size (default) is returned to indicate that the original size of the icon is used.
     * @return size specified by @see setSize
     */
    QSize size() const;

    void setScale(float scale);
    float scale() const;

    int heading() const;
    void setHeading( int heading );
    
    RemoteIconLoader *remoteIconLoader() const;

    /*
     * Serializable methods
     */
    void pack( QDataStream& stream ) const override;
    void unpack( QDataStream& stream ) override;

  private:
    GeoDataIconStylePrivate * const d;
};

}

#endif
