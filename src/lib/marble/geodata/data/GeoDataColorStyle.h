// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Murad Tagirov <tmurad@gmail.com>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//


#ifndef MARBLE_GEODATACOLORSTYLE_H
#define MARBLE_GEODATACOLORSTYLE_H

#include "GeoDataObject.h"

#include "geodata_export.h"

class QColor;

namespace Marble
{

class GeoDataColorStylePrivate;

/**
 * @short  an abstract base class for various style classes
 *
 * A GeoDataColorStyle is an abstract class that is the base class for
 * a number of different style classes. It provides settings for
 * specifying the color and color mode of the extended style classes.
 * A GeoDataColorStyle should never be instantiated directly.
 *
 * The color contains RGBA, (Red, Green, Blue, Alpha).  Color and
 * opacity (alpha) values have a range of 0 to 255 (00 to ff). For
 * alpha, 00 is fully transparent and ff is fully opaque.  For
 * example, if you want to apply a blue color with 50 percent opacity
 * to an overlay, you would specify the following:
 * 7fff0000, where alpha=0x7f, blue=0xff, green=0x00,
 * and red=0x00.
 *
 * The color mode can either be <b>normal</b> (no effect) or
 * <b>random</b>. A value of <b>random</b> applies a random linear scale to
 * the base color as follows.
 *
 * To achieve a truly random selection of colors, specify a base color
 * of white (ffffffff). If you specify a single color component (for
 * example, a value of ff0000ff for red), random color values for that
 * one component (red) will be selected. In this case, the values
 * would range from 00 (black) to ff (full red).  If you specify
 * values for two or for all three color components, a random linear
 * scale is applied to each color component, with results ranging from
 * black to the maximum values specified for each component.  The
 * opacity of a color comes from the alpha component of color and is
 * never randomized.
 *
 * @see GeoDataIconStyle
 * @see GeoDataLabelStyle
 * @see GeoDataLineStyle
 */
class GEODATA_EXPORT GeoDataColorStyle : public GeoDataObject
{
  public:
    /// Provides type information for downcasting a GeoData
    const char* nodeType() const override;

    /**
     * @brief  Set a new color
     * @param  value  the new color value
     */
    void setColor( const QColor &value );
    /// Return the color component
    QColor color() const;

    /// The color mode
    enum ColorMode { Normal, Random };

    /**
      * @brief Returns the color that should be painted: Either color() or a randomized
      * version of it, depending on the colorMode() setting. Randomization happens once
      * per setColor() call, i.e. repeated calls to paintedColor always return the same
      * color unless setColor is called in between.
      */
    QColor paintedColor() const;

    /**
     * @brief  Set a new color mode
     * @param  colorMode  the new color mode value
     */
    void setColorMode(ColorMode colorMode);
    /// Return the color mode
    ColorMode colorMode() const;


    /**
    * assignment operator
    * @param other the other colorstyle
    */
    GeoDataColorStyle& operator=( const GeoDataColorStyle& other );
    bool operator==( const GeoDataColorStyle &other ) const;
    bool operator!=( const GeoDataColorStyle &other ) const;

    /**
     * @brief Serialize the style to a stream
     * @param  stream  the stream
     */
    void pack( QDataStream& stream ) const override;
    /**
     * @brief  Unserialize the style from a stream
     * @param  stream  the stream
     */
    void unpack( QDataStream& stream ) override;

    GeoDataColorStyle();
    GeoDataColorStyle( const GeoDataColorStyle& other );

    ~GeoDataColorStyle() override;

    /**
     * @return Returns a foreground color suitable for e.g. text display on top of the given background color
     */
    static QString contrastColor(const QColor &color);

  private:
    GeoDataColorStylePrivate * const d;
};

}

#endif
