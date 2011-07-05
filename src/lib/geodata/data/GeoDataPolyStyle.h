//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef MARBLE_GEODATAPOLYSTYLE_H
#define MARBLE_GEODATAPOLYSTYLE_H


#include <QtGui/QFont>

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

    ~GeoDataPolyStyle();
    
    /**
    * @brief assignment operator
    */
    GeoDataPolyStyle& operator=( const GeoDataPolyStyle& other );

    /// Provides type information for downcasting a GeoNode
    virtual const char* nodeType() const;

    /**
     * @brief Set whether to fill the polygon
     * @param  fill  
     */
    void setFill( const bool &fill );
    /**
     * @brief Return true if polygons get filled
     * @return whether to fill
     */
    bool fill() const;

    /**
     * @brief Set whether to draw the outline
     * @param  outline  
     */
    void setOutline( const bool &outline );
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
     * @brief  Serialize the style to a stream.
     * @param  stream  the stream
     */
    virtual void pack( QDataStream& stream ) const;
    /**
     * @brief  Unserialize the style from a stream
     * @param  stream  the stream
     */
    virtual void unpack( QDataStream& stream );

  private:
    GeoDataPolyStylePrivate * const d;
};

}

#endif
