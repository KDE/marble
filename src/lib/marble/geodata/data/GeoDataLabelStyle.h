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


#ifndef MARBLE_GEODATALABELSTYLE_H
#define MARBLE_GEODATALABELSTYLE_H

#include "GeoDataColorStyle.h"

#include "geodata_export.h"

class QFont;

namespace Marble
{

class GeoDataLabelStylePrivate;

/**
 * @short specifies how the <b>name</b> of a GeoDataFeature is drawn
 *
 * A GeoDataLabelStyle specifies how the <b>name</b> of a
 * GeoDataFeature is drawn in the viewer. A custom color, color mode
 * (both inherited from GeoDataColorStyle, and scale for the label
 * (name) can be specified.
 */
class GEODATA_EXPORT GeoDataLabelStyle : public GeoDataColorStyle
{
  public:
    enum Alignment{Corner, Center, Right /*, Left, Below  */};

    /// Construct a new GeoDataLabelStyle
    GeoDataLabelStyle();
    GeoDataLabelStyle( const GeoDataLabelStyle& other );
    /**
     * @brief Construct a new GeoDataLabelStyle
     * @param  font   the font to use when showing the name
     * @param  color  the color to use when showing the name @see GeoDataColorStyle
     */
    GeoDataLabelStyle( const QFont &font, const QColor &color );

    ~GeoDataLabelStyle();
    
    /**
    * @brief assignment operator
    */
    GeoDataLabelStyle& operator=( const GeoDataLabelStyle& other );

    bool operator==( const GeoDataLabelStyle &other ) const;
    bool operator!=( const GeoDataLabelStyle &other ) const;

    /// Provides type information for downcasting a GeoData
    virtual const char* nodeType() const;

    /**
     * @brief Set the scale of the label
     * @param  scale  the new scale
     */
    void setScale(float scale);
    /**
     * @brief Return the current scale of the label
     * @return  the current scale
     */
    float scale() const;
    /**
     * @brief Set the alignment of the label
     * @param  int the alignment
     */
    void setAlignment ( GeoDataLabelStyle::Alignment alignment );
    /**
     * @brief Return the alignment of the label
     * @return  the current alignment
     */
    GeoDataLabelStyle::Alignment alignment() const;
    /**
     * @brief Set the font of the label
     * @param  font  the new font
     */
    void setFont( const QFont &font );
    /**
     * @brief Return the current font of the label
     * @return  the current font
     */
    QFont font() const;

    /**
     * @brief Return the scaled font of the label
     * @return  the scaled font
     */
    QFont scaledFont() const;

    /**
     * @brief Return true if the text of the label should glow, false otherwise
     * @see setGlow
     */
    bool glow() const;

    /**
     * @brief Enable or disable a glow effect around the text of the label.
     *
     * The default is false.
     *
     * Note that this is not a KML property.
     */
    void setGlow( bool on );

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
    GeoDataLabelStylePrivate * const d;
};

}

#endif
