//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef PLACEMARKTEXTANNOTATION_H
#define PLACEMARKTEXTANNOTATION_H

#include "TextAnnotation.h"
#include "TmpGraphicsItem.h"

namespace Marble
{

class GeoWidgetBubble;
class TextEditor;
class GeoDataPlacemark;

class PlacemarkTextAnnotation : public TextAnnotation, public TmpGraphicsItem
{
public:
    PlacemarkTextAnnotation();

    //GeoGraphicsItem
    virtual QRect screenBounding();
    virtual void paint( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer = 0 );
    virtual QVariant itemChange(GeoGraphicsItemChange c, QVariant v);

    //TextAnnotation
    virtual QString name();
    virtual QString description();
    virtual GeoDataGeometry geometry();

private:
    GeoWidgetBubble* bubble;
    TextEditor* m_textEditor;

protected:
    virtual bool mousePressEvent( QMouseEvent *event );
};

}

#endif // PLACEMARKTEXTANNOTATION_H
