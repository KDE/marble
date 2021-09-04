// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef MARBLE_GEODATAPOLYSTYLE_H
#define MARBLE_GEODATAPOLYSTYLE_H

#include "GeoDataColorStyle.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataPolyStylePrivate;

/**
 * @short specifies the style how polygons are drawn
 *
 * A GeoDataPolyStyle specifies how Polygons are drawn in the viewer. 
 * A custom color, color mode (both inherited from GeoDataColorStyle) 
 * and two boolean values whether to fill and whether to draw the outline.
 */
class GEODATA_EXPORT GeoDataPolyStyle : public GeoDataColorStyle
{
  public:
    /// Construct a new GeoDataPolyStyle
    GeoDataPolyStyle();
    GeoDataPolyStyle( const GeoDataPolyStyle& other );
    /**
     * @brief Construct a new GeoDataPolyStyle
     * @param  color  the color to use when showing the name @see GeoDataColorStyle
     */
    explicit GeoDataPolyStyle( const QColor &color );

    ~GeoDataPolyStyle() override;
    
    /**
    * @brief assignment operator
    */
    GeoDataPolyStyle& operator=( const GeoDataPolyStyle& other );

    bool operator==( const GeoDataPolyStyle &other ) const;
    bool operator!=( const GeoDataPolyStyle &other ) const;

    /// Provides type information for downcasting a GeoNode
    const char* nodeType() const override;

    /**
     * @brief Set whether to fill the polygon
     * @param  fill  
     */
    void setFill(bool fill);
    /**
     * @brief Return true if polygons get filled
     * @return whether to fill
     */
    bool fill() const;

    /**
     * @brief Set whether to draw the outline
     * @param  outline  
     */
    void setOutline(bool outline);
    /**
     * @brief Return true if outlines of polygons get drawn
     * @return whether outline is drawn
     */
    bool outline() const;
    
    /**
     * @brief Set brush style
     * @param style  
     */
    void setBrushStyle( const Qt::BrushStyle style );
    /**
     * @brief Return brush style
     * @return brush style.
     */
    Qt::BrushStyle brushStyle() const;

    /**
     * @brief Set the color index which will be used to assign color to brush
     * @param colorIndex The value of color index
     */
    void setColorIndex( quint8 colorIndex );

    /**
     * @brief Return the value of color index
     * @return Color index
     */
    quint8 colorIndex() const;

    void setTexturePath( const QString &path );

    QString texturePath() const;

    QImage textureImage() const;

    /**
     * @brief  Serialize the style to a stream.
     * @param  stream  the stream
     */
    void pack( QDataStream& stream ) const override;
    /**
     * @brief  Unserialize the style from a stream
     * @param  stream  the stream
     */
    void unpack( QDataStream& stream ) override;

  private:
    GeoDataPolyStylePrivate * const d;
};

}

#endif
