//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Torsten Rahn    <rahn@kde.org>
//

#ifndef MARBLE_BATCHEDPLACEMARKRENDERER_H
#define MARBLE_BATCHEDPLACEMARKRENDERER_H

#include "marble_export.h"

#include <QColor>
#include <QPoint>
#include <QVector>

namespace Marble
{

class GeoPainter;

struct TextFragment;

/**
 * @short This class renders placemarks where the positioning
 *        criteria differ from those inside the default Placemark
 *        layer / layout classes (e.g. house numbers).
 *
 */

class MARBLE_EXPORT BatchedPlacemarkRenderer
{
 public:
    enum Frame {
        NoOptions = 0x0,
        RoundFrame = 0x1
    };

    Q_DECLARE_FLAGS(Frames, Frame)

    BatchedPlacemarkRenderer();
    ~BatchedPlacemarkRenderer();

    void addTextFragment( const QPoint& targetPosition, const QString& text,
                          const qreal fontSize, const QColor& color,
                          const QFlags<BatchedPlacemarkRenderer::Frames> & flags );
    void clearTextFragments();
    void drawTextFragments(GeoPainter *painter);

private:
    QVector<TextFragment> m_textFragments;
};

struct TextFragment
{
    QString text;
    QPoint position;
    qreal fontSize;
    QColor color;
    QFlags<BatchedPlacemarkRenderer::Frames> flags;
};


}

#endif
