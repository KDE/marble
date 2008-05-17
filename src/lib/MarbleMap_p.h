//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Carlos Licea <carlos.licea@kdemail.net>
//

#ifndef MARBLEMAP_P_H
#define MARBLEMAP_P_H


#include "ViewParams.h"
#include "TextureColorizer.h"
#include "CrossHairFloatItem.h"
#include "MapScaleFloatItem.h"


class MarbleMap;
class MarbleModel;


class MarbleMapPrivate
{
    friend class MarbleWidget;

 public:
    explicit MarbleMapPrivate( MarbleMap *parent );

    inline static int toLogScale( int zoom ) { return (int)(200.0 * log( (double)zoom ) ); }
    inline static int fromLogScale( int zoom ) { return (int)pow( M_E, ( (double)zoom / 200.0 ) ); }

    void  construct();
    void  drawAtmosphere();
    void  drawFog(QPainter &painter);
    void  setBoundingBox();
    void  doResize();

    MarbleMap       *m_parent;

    // The model we are showing.
    MarbleModel     *m_model;
    bool             m_modelIsOwned;

    int              m_width;
    int              m_height;
    ViewParams       m_viewParams;
    bool             m_justModified; // FIXME: Rename to isDirty

    // The home position
    GeoDataPoint     m_homePoint;
    int              m_homeZoom;

    // zoom related
    int              m_logzoom;
    int              m_zoomStep;

    TextureColorizer  *m_sealegend;

    // Parameters for the maps appearance.

    bool             m_showFrameRate;

    // Parts of the image in the Map
    CrossHairFloatItem         m_crosshair;
    MapScaleFloatItem          m_mapscale; // Shown in the lower left

    // Tools
    MeasureTool     *m_measureTool;

    // Cache related
    quint64          m_persistentTileCacheLimit;
    quint64          m_volatileTileCacheLimit;
};

#endif
