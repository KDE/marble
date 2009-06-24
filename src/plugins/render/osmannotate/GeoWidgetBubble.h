#ifndef GEOWIDGETBUBBLE_H
#define GEOWIDGETBUBBLE_H

#include "TmpGraphicsItem.h"

namespace Marble {


class GeoWidgetBubble : public TmpGraphicsItem
{
public:
    GeoWidgetBubble();
    GeoWidgetBubble( TmpGraphicsItem* parent );

    virtual void paint(GeoPainter* p, ViewportParams* v,
                       const QString& renderPos, GeoSceneLayer* l);

    void setWidget( QWidget* w );
    QWidget* getWidget();

    void initWidget( QWidget* parent );

    bool isInitalised();

private:
    QWidget* widget;
    bool initalised;


};

}

#endif // GEOWIDGETBUBBLE_H
