//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// The TextureColorizer maps the Elevationvalues to Legend Colors.
//

#ifndef MARBLE_TEXTURECOLORIZER_H
#define MARBLE_TEXTURECOLORIZER_H

#include "MarbleGlobal.h"
#include "GeoDataDocument.h"
#include "GeoPainter.h"

#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtGui/QPen>
#include <QtGui/QBrush>

namespace Marble
{

class VectorComposer;
class ViewportParams;

class TextureColorizer
{
 public:
    TextureColorizer( const QString &seafile,
                      const QString &landfile,
                      VectorComposer *veccomposer );

    virtual ~TextureColorizer(){}

    void setCoastDocument( GeoDataDocument* coastDocument );

    void setGlacierDocument( GeoDataDocument* glacierDocument );

    void setLakeDocument( GeoDataDocument* lakeDocument );

    void setShowRelief( bool show );

    void drawIndividualDocument( GeoPainter *painter, GeoDataDocument *document );

    void drawTextureMap( GeoPainter *painter );

    void colorize( QImage *origimg, const ViewportParams *viewport, MapQuality mapQuality );

 private:
    VectorComposer *const m_veccomposer;
    QString m_seafile;
    QString m_landfile;
    GeoDataDocument* m_coastDocument;
    GeoDataDocument* m_glacierDocument;
    GeoDataDocument* m_lakeDocument;
    QImage m_coastImage;
    uint texturepalette[16][512];
    bool m_showRelief;
    QPen        m_textureLandPen;
    QBrush      m_textureLandBrush;
    QBrush      m_textureGlacierBrush;
    QBrush      m_textureLakeBrush;
};

}

#endif
