//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
//


#ifndef GEODATALABELSTYLE_H
#define GEODATALABELSTYLE_H


#include <QtGui/QFont>

#include "GeoDataColorStyle.h"


/**
 * @short specifies how the <b>name</b> of a GeoDataFeature is drawn
 *
 * A GeoDataLabelStyle specifies how the <b>name</b> of a
 * GeoDataFeature is drawn in the viewer. A custom color, color mode
 * (both inherited from GeoDataColorStyle, and scale for the label
 * (name) can be specified.
 */
class GeoDataLabelStyle : public GeoDataColorStyle
{
  public:
    /// Construct a new GeoDataLabelStyle
    GeoDataLabelStyle();
    /**
     * @brief Construct a new GeoDataLabelStyle
     * @param  font   the font to use when showing the name
     * @param  color  the color to use when showing the name @see GeoDataColorStyle
     */
    GeoDataLabelStyle( const QFont &font, const QColor &color );

    /**
     * @brief Set the scale of the label
     * @param  scale  the new scale
     */
    void setScale( const float &scale );
    /**
     * @brief Return the current scale of the label
     * @return  the current scale
     */
    float scale() const;

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
     * @brief  Serialize the style to a stream.
     * @param  stream  the stream
     */
    virtual void pack( QDataStream& stream ) const;
    /**
     * @brief  Unserialize the style from a stream
     * @param  stream  the stream
     */
    virtual void unpack( QDataStream& stream );

  protected:
    /// The current scale of the label
    float  m_scale;
    /// The current font of the label
    QFont  m_font;   // Not a KML property
};

#endif // GEODATALABELSTYLE_H
