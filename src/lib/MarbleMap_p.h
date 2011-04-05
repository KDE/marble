//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Carlos Licea <carlos.licea@kdemail.net>
//

#ifndef MARBLE_MARBLEMAP_P_H
#define MARBLE_MARBLEMAP_P_H

#include "AtmosphereLayer.h"
#include "FogLayer.h"
#include "GeometryLayer.h"
#include "LayerManager.h"
#include "MeasureTool.h"
#include "PlacemarkLayout.h"
#include "TextureLayer.h"
#include "VectorComposer.h"
#include "ViewParams.h"

class QPainter;
class QRect;

namespace Marble
{

class MarbleMap;
class MarbleModel;
class TextureColorizer;


class MarbleMapPrivate
{
    friend class MarbleWidget;

 public:
    explicit MarbleMapPrivate( MarbleMap *parent, MarbleModel *model );

    void construct();

    void paintMarbleSplash( GeoPainter &painter, QRect &dirtyRect );

    void setBoundingBox();

    MarbleMap       *m_parent;

    // The model we are showing.
    MarbleModel     *const m_model;
    bool             m_modelIsOwned;

    ViewParams       m_viewParams;
    bool             m_backgroundVisible;

    LayerManager     m_layerManager;
    GeometryLayer           *m_geometryLayer;
    AtmosphereLayer          m_atmosphereLayer;
    FogLayer                 m_fogLayer;
    TextureLayer     m_textureLayer;
    PlacemarkLayout  m_placemarkLayout;
    VectorComposer   m_veccomposer;
    MeasureTool      m_measureTool;
};

}

#endif
