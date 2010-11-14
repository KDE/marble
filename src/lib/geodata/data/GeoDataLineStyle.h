//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef MARBLE_GEODATALINESTYLE_H
#define MARBLE_GEODATALINESTYLE_H


#include <QtGui/QFont>

#include "GeoDataColorStyle.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataLineStylePrivate;

/**
 * @short specifies the style how lines are drawn
 *
 * A GeoDataLineStyle specifies how the <b>name</b> of a
 * GeoDataFeature is drawn in the viewer. A custom color, color mode
 * (both inherited from GeoDataColorStyle) and width for the width
 * of the line.
 */
class GEODATA_EXPORT GeoDataLineStyle : public GeoDataColorStyle
{
  public:
    /// Construct a new GeoDataLineStyle
    GeoDataLineStyle();
    GeoDataLineStyle( const GeoDataLineStyle& other );
    /**
     * @brief Construct a new GeoDataLineStyle
     * @param  color  the color to use when showing the name @see GeoDataColorStyle
     */
    explicit GeoDataLineStyle( const QColor &color );

    ~GeoDataLineStyle();

    /**
    * @brief assignment operator
    */
    GeoDataLineStyle& operator=( const GeoDataLineStyle& other );

    /// Provides type information for downcasting a GeoData
    virtual QString nodeType() const;

    /**
     * @brief Set the width of the line
     * @param  width  the new width
     */
    void setWidth( const float &width );
    /**
     * @brief Return the current width of the line
     * @return the current width
     */
    float width() const;

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
    GeoDataLineStylePrivate * const d;
};

}

#endif
