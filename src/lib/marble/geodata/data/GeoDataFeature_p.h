//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAFEATUREPRIVATE_H
#define MARBLE_GEODATAFEATUREPRIVATE_H

#include <QString>
#include <QAtomicInt>

#include "GeoDataExtendedData.h"
#include "GeoDataAbstractView.h"
#include "GeoDataFeature.h"
#include "GeoDataRegion.h"
#include "GeoDataTimeStamp.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataTypes.h"
#include "GeoDataStyle.h"
#include "GeoDataSnippet.h"
#include "MarbleDirs.h"

namespace Marble
{

class GeoDataFeaturePrivate
{
  public:
    GeoDataFeaturePrivate() :
        m_name(),
        m_snippet(),
        m_description(),
        m_descriptionCDATA(),
        m_address(),
        m_phoneNumber(),
        m_styleUrl(),
        m_abstractView( 0 ),
        m_popularity( 0 ),
        m_zoomLevel( 1 ),
        m_visible( true ),
        m_visualCategory( GeoDataFeature::Default ),
        m_role(" "),
        m_style( 0 ),
        m_styleMap( 0 ),
        m_extendedData(),
        m_timeSpan(),
        m_timeStamp(),
        m_region(),
        ref( 0 )
    {
    }

    GeoDataFeaturePrivate( const GeoDataFeaturePrivate& other ) :
        m_name( other.m_name ),
        m_snippet( other.m_snippet ),
        m_description( other.m_description ),
        m_descriptionCDATA( other.m_descriptionCDATA),
        m_address( other.m_address ),
        m_phoneNumber( other.m_phoneNumber ),
        m_styleUrl( other.m_styleUrl ),
        m_abstractView( other.m_abstractView ),
        m_popularity( other.m_popularity ),
        m_zoomLevel( other.m_zoomLevel ),
        m_visible( other.m_visible ),
        m_visualCategory( other.m_visualCategory ),
        m_role( other.m_role ),
        m_style( other.m_style ),               //FIXME: both style and stylemap need to be reworked internally!!!!
        m_styleMap( other.m_styleMap ),
        m_extendedData( other.m_extendedData ),
        m_timeSpan( other.m_timeSpan ),
        m_timeStamp( other.m_timeStamp ),
        m_region( other.m_region ),
        ref( 0 )
    {
    }

    GeoDataFeaturePrivate& operator=( const GeoDataFeaturePrivate& other )
    {
        m_name = other.m_name;
        m_snippet = other.m_snippet;
        m_description = other.m_description;
        m_descriptionCDATA = other.m_descriptionCDATA;
        m_address = other.m_address;
        m_phoneNumber = other.m_phoneNumber;
        m_styleUrl = other.m_styleUrl;
        m_abstractView = other.m_abstractView;
        m_popularity = other.m_popularity;
        m_zoomLevel = other.m_zoomLevel;
        m_visible = other.m_visible;
        m_role = other.m_role;
        m_style = other.m_style;
        m_styleMap = other.m_styleMap;
        m_timeSpan = other.m_timeSpan;
        m_timeStamp = other.m_timeStamp;
        m_visualCategory = other.m_visualCategory;
        m_extendedData = other.m_extendedData;
        m_region = other.m_region;
        return *this;
    }
    
    virtual GeoDataFeaturePrivate* copy()
    { 
        GeoDataFeaturePrivate* copy = new GeoDataFeaturePrivate;
        *copy = *this;
        return copy;
    }

    virtual EnumFeatureId featureId() const
    {
        return InvalidFeatureId;
    }

    virtual ~GeoDataFeaturePrivate()
    {
    }

    virtual const char* nodeType() const
    {
        return GeoDataTypes::GeoDataFeatureType;
    }

    static void initializeDefaultStyles();

    static GeoDataStyle::Ptr createOsmPOIStyle( const QFont &font, const QString &bitmap,
                                         const QColor &textColor = Qt::black,
                                         const QColor &color = QColor( 0xBE, 0xAD, 0xAD ),
                                         const QColor &outline = QColor( 0xBE, 0xAD, 0xAD ).darker()
                                         )
    {
        GeoDataStyle::Ptr style =  createStyle(1, 0, color, outline, true, true, Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false);
        QString const imagePath = MarbleDirs::path( "bitmaps/osmcarto/symbols/48/" + bitmap + ".png" );
        style->setIconStyle( GeoDataIconStyle( imagePath) );
        style->iconStyle().setScale(0.67);
        style->setLabelStyle( GeoDataLabelStyle( font, textColor ) );
        style->labelStyle().setAlignment(GeoDataLabelStyle::Center);
        return style;
    }
    
    static GeoDataStyle::Ptr createHighwayStyle( const QString &bitmap, const QColor& color, const QColor& outlineColor,
                                             const QFont& font = QFont(QLatin1String("Arial")), const QColor& fontColor = Qt::black,
                                             qreal width = 1, qreal realWidth = 0.0,
                                             Qt::PenStyle penStyle = Qt::SolidLine,
                                             Qt::PenCapStyle capStyle = Qt::RoundCap,
                                             bool lineBackground = false)
    {
        GeoDataStyle::Ptr style = createStyle( width, realWidth, color, outlineColor, true, true,
                                           Qt::SolidPattern, penStyle, capStyle, lineBackground, QVector< qreal >(),
                                           font, fontColor );
        if( !bitmap.isEmpty() ) {
            style->setIconStyle( GeoDataIconStyle( MarbleDirs::path( "bitmaps/" + bitmap + ".png" ) ) );
        }
        return style;
    }

    static GeoDataStyle::Ptr createWayStyle( const QColor& color, const QColor& outlineColor,
                                         bool fill = true, bool outline = true,
                                         Qt::BrushStyle brushStyle = Qt::SolidPattern,
                                         const QString& texturePath = QString())
    {
        return createStyle( 1, 0, color, outlineColor, fill, outline, brushStyle, Qt::SolidLine, Qt::RoundCap, false, QVector< qreal >(), QFont("Arial"), Qt::black, texturePath );
    }

    static GeoDataStyle::Ptr createStyle( qreal width, qreal realWidth, const QColor& color,
                                      const QColor& outlineColor, bool fill, bool outline,
                                      Qt::BrushStyle brushStyle, Qt::PenStyle penStyle,
                                      Qt::PenCapStyle capStyle, bool lineBackground,
                                      const QVector< qreal >& dashPattern = QVector< qreal >(),
                                      const QFont& font = QFont(QLatin1String("Arial")), const QColor& fontColor = Qt::black,
                                      const QString& texturePath = QString())
    {
        GeoDataStyle *style = new GeoDataStyle;
        GeoDataLineStyle lineStyle( outlineColor );
        lineStyle.setCapStyle( capStyle );
        lineStyle.setPenStyle( penStyle );
        lineStyle.setWidth( width );
        lineStyle.setPhysicalWidth( realWidth );
        lineStyle.setBackground( lineBackground );
        lineStyle.setDashPattern( dashPattern );
        GeoDataPolyStyle polyStyle( color );
        polyStyle.setOutline( outline );
        polyStyle.setFill( fill );
        polyStyle.setBrushStyle( brushStyle );
        polyStyle.setTexturePath( texturePath );
        GeoDataLabelStyle labelStyle(font, fontColor);
        style->setLineStyle( lineStyle );
        style->setPolyStyle( polyStyle );
        style->setLabelStyle( labelStyle );
        return GeoDataStyle::Ptr(style);
    }

    QString             m_name;         // Name of the feature. Is shown on screen
    GeoDataSnippet      m_snippet;      // Snippet of the feature.
    QString             m_description;  // A longer textual description
    bool                m_descriptionCDATA; // True if description should be considered CDATA
    QString             m_address;      // The address.  Optional
    QString             m_phoneNumber;  // Phone         Optional
    QString             m_styleUrl;     // styleUrl     Url#tag to a document wide style
    GeoDataAbstractView* m_abstractView; // AbstractView  Optional
    qint64              m_popularity;   // Population/Area/Altitude depending on placemark(!)
    int                 m_zoomLevel;    // Zoom Level of the feature

    bool        m_visible;      // True if this feature should be shown.
    GeoDataFeature::GeoDataVisualCategory  m_visualCategory; // the visual category


    QString       m_role;

    GeoDataStyle::Ptr m_style;
    const GeoDataStyleMap* m_styleMap;

    GeoDataExtendedData m_extendedData;

    GeoDataTimeSpan  m_timeSpan;
    GeoDataTimeStamp m_timeStamp;

    GeoDataRegion m_region;
    
    QAtomicInt  ref;

    // Static members
    static QFont         s_defaultFont;
    static QColor        s_defaultLabelColor;

    static GeoDataStyle::Ptr s_defaultStyle[GeoDataFeature::LastIndex];
    static bool          s_defaultStyleInitialized;
};

} // namespace Marble

#endif

