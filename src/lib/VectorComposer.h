//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


//
// The VectorComposer maps the data stored as polylines onto the
// respective projection.
//
// Author: Torsten Rahn
//


#ifndef VECTORCOMPOSER_H
#define VECTORCOMPOSER_H


#include <QtGui/QBrush>
#include <QtGui/QImage>
#include <QtGui/QPen>
#include <QtGui/QPixmap>


class GeoPainter;
class PntMap;
class VectorMap;
class ViewParams;


class VectorComposer
{
 public:
    VectorComposer();
    virtual ~VectorComposer();

    void  drawTextureMap( ViewParams *viewParams );
    void  paintBaseVectorMap( GeoPainter*, ViewParams* );
    void  paintVectorMap(GeoPainter*, ViewParams* );
    void  resizeMap( int width, int height );

    /**
     * @brief  Set color of the oceans
     * @param  color  ocean color
     */
    void setOceanColor( const QColor& color ){ 
        m_oceanBrush = QBrush( color );
        m_oceanPen = QPen( Qt::NoPen );
    }

    /**
     * @brief  Set color of the land
     * @param  color  land color
     */
    void setLandColor( const QColor& color ){ 
        m_landBrush = QBrush( color );
        m_landPen = QPen( Qt::NoPen );
    }

    /**
     * @brief  Set color of the lakes
     * @param  color  lake color
     */
    void setLakeColor( const QColor& color ){ 
        m_lakeBrush = QBrush( color );
        m_lakePen = QPen( Qt::NoPen );
    }

    /**
     * @brief  Set color of the rivers
     * @param  color  river color
     */
    void setRiverColor( const QColor& color ){ 
        m_riverBrush = QBrush( Qt::NoBrush );
        m_riverPen = QPen( color );
    }

    /**
     * @brief  Set color of the country borders
     * @param  color  country border color
     */
    void setCountryBorderColor( const QColor& color ){ 
        m_countryBrush = QBrush( Qt::NoBrush );
        m_countryPen = QPen( color );
    }

    /**
     * @brief  Set color of the state borders
     * @param  color  state border color
     */
    void setStateBorderColor( const QColor& color ){ 
        m_stateBrush = QBrush( Qt::NoBrush );
        m_statePen = QPen( color );
        m_statePen.setStyle( Qt::DotLine );
    }

 private:
    Q_DISABLE_COPY( VectorComposer )
    VectorMap  *m_vectorMap;

    PntMap     *m_coastLines;

    PntMap     *m_islands;
    PntMap     *m_lakeislands;
    PntMap     *m_lakes;
    PntMap     *m_glaciers;
    PntMap     *m_rivers;

    PntMap     *m_countries;    // The country borders
    PntMap     *m_usaStates;    // The states of the USA

    PntMap     *m_dateLine;

    QPen        m_oceanPen;
    QBrush      m_oceanBrush;

    QPen        m_landPen;
    QBrush      m_landBrush;

    QPen        m_lakePen;
    QBrush      m_lakeBrush;

    QPen        m_countryPen;
    QBrush      m_countryBrush;

    QPen        m_statePen;
    QBrush      m_stateBrush;

    QPen        m_riverPen;
    QBrush      m_riverBrush;

    QPen        m_textureLandPen;
    QBrush      m_textureLandBrush;
    QPen        m_textureBorderPen;
    QBrush      m_textureGlacierBrush;
    QBrush      m_textureLakeBrush;

    QPen        m_dateLinePen;
    QBrush      m_dateLineBrush;

    QVector<qreal> m_dashes;
};

#endif // VECTORCOMPOSER_H
