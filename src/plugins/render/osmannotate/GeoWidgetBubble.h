#ifndef GEOWIDGETBUBBLE_H
#define GEOWIDGETBUBBLE_H

#include <QtCore/QString>
#include <QtGui/QWidget>

namespace Marble {

class GeoPainter;
class ViewportParams;
class GeoSceneLayer;


class GeoWidgetBubble
{
public:
    GeoWidgetBubble();

    void paint(GeoPainter* p, ViewportParams* v, const QString& renderPos,
               GeoSceneLayer* l);

    void setGeoWidget( QWidget* w );
    QWidget* getGeoWidget();

    void initaliseMarbleWidget( QWidget* parent );
    bool marbleWidgetIsInitalised();

    void moveTo( QPoint screenPos );
    void hide();

private:
    QWidget* m_widget;
    bool marbleWidgetInitalised;
    QPoint m_offset;
    QPoint m_screenPosition;


};

}

#endif // GEOWIDGETBUBBLE_H
