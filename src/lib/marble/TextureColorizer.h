// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2004-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//

//
// The TextureColorizer maps the Elevationvalues to Legend Colors.
//

#ifndef MARBLE_TEXTURECOLORIZER_H
#define MARBLE_TEXTURECOLORIZER_H

#include "GeoDataDocument.h"
#include "MarbleGlobal.h"

#include <QColor>
#include <QImage>
#include <QString>

namespace Marble
{

class GeoPainter;
class ViewportParams;

class TextureColorizer
{
public:
    TextureColorizer(const QString &seafile, const QString &landfile);

    virtual ~TextureColorizer() = default;

    void addSeaDocument(const GeoDataDocument *seaDocument);

    void addLandDocument(const GeoDataDocument *landDocument);

    void setShowRelief(bool show);

    static void drawIndividualDocument(GeoPainter *painter, const GeoDataDocument *document);

    void drawTextureMap(GeoPainter *painter);

    void colorize(QImage *origimg, const ViewportParams *viewport, MapQuality mapQuality);

    void setPixel(const QRgb *coastData, QRgb *writeData, int bump, uchar grey);

private:
    QString m_seafile;
    QString m_landfile;
    QList<const GeoDataDocument *> m_seaDocuments;
    QList<const GeoDataDocument *> m_landDocuments;
    QImage m_coastImage;
    uint texturepalette[16][512];
    bool m_showRelief;
    QRgb m_landColor;
    QRgb m_seaColor;
};

}

#endif
