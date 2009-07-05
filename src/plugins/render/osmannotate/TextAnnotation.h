//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef TEXTANNOTATION_H
#define TEXTANNOTATION_H


#include "TmpGraphicsItem.h"
namespace Marble{

class GeoWidgetBubble;
class TextEditor;

class TextAnnotation : public TmpGraphicsItem
{
public:
    TextAnnotation();

    virtual QRect screenBounding();
    virtual void geoBounding(qreal angularResoluiton);
    virtual void paint( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer = 0 );
    virtual bool isGeoProjected();
    virtual QVariant itemChange(GeoGraphicsItemChange c, QVariant v);

protected:
    virtual bool mousePressEvent( QMouseEvent *event );

private:
    GeoWidgetBubble* bubble;
    TextEditor* m_textEditor;
};

}

#endif // TEXTANNOTATION_H
