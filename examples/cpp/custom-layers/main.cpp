//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#include <marble/MarbleWidget.h>
#include <marble/MarbleMap.h>
#include <marble/MarbleModel.h>
#include <marble/GeoPainter.h>
#include <marble/GeoDataLineString.h>
#include <marble/LayerInterface.h>

#include <QTime>
#include <QTimer>
#include <QApplication>
#include <QKeyEvent>

using namespace Marble;

class MyPaintLayer : public QObject, public LayerInterface
{
public:
    // Constructor
    MyPaintLayer(MarbleWidget* widget);

    // Implemented from LayerInterface
    virtual QStringList renderPosition() const;

    // Implemented from LayerInterface
    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
                        const QString &renderPos, GeoSceneLayer *layer);

    // Overriding QObject
    virtual bool eventFilter(QObject *obj, QEvent *event);

    static GeoDataCoordinates approximate(const GeoDataCoordinates &base, qreal angle, qreal dist);

private:
    MarbleWidget* m_widget;

    int m_index;
};

MyPaintLayer::MyPaintLayer(MarbleWidget* widget) : m_widget(widget), m_index(0)
{
    // nothing to do
}

QStringList MyPaintLayer::renderPosition() const
{
    // We will paint in exactly one of the following layers.
    // The current one can be changed by pressing the '+' key
    const QStringList layers = QStringList()
        << QStringLiteral("SURFACE")
        << QStringLiteral("HOVERS_ABOVE_SURFACE")
        << QStringLiteral("ORBIT")
        << QStringLiteral("USER_TOOLS")
        << QStringLiteral("STARS");

    int index = m_index % layers.size();
    return QStringList() << layers.at(index);
}

bool MyPaintLayer::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)

    // Adjust the current layer when '+' is pressed
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Plus) {
            ++m_index;
            return true;
        }
    }

    return false;
}

GeoDataCoordinates MyPaintLayer::approximate(const GeoDataCoordinates &base, qreal angle, qreal dist)
{
    // This is just a rough estimation that ignores projections.
    // It only works for short distances. Don't use in real code.
    GeoDataCoordinates::Unit deg = GeoDataCoordinates::Degree;
    return GeoDataCoordinates ( base.longitude(deg) + 1.5 * dist * sin(angle),
				base.latitude(deg) + dist * cos(angle), 0.0, deg);
}

bool MyPaintLayer::render( GeoPainter *painter, ViewportParams *viewport,
    const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED(viewport)
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)

    // Have window title reflect the current paint layer
    m_widget->setWindowTitle(renderPosition().first());
    GeoDataCoordinates home(8.4, 48.0, 0.0, GeoDataCoordinates::Degree);
    QTime now = QTime::currentTime();

    painter->setRenderHint(QPainter::Antialiasing, true);

    // Large circle built by 60 small circles
    painter->setPen( QPen(QBrush(QColor::fromRgb(255,255,255,200)), 3.0, Qt::SolidLine, Qt::RoundCap ) );
    for (int i=0; i<60; ++i)
        painter->drawEllipse(approximate(home, M_PI * i / 30.0, 1.0), 5, 5);

    // hour, minute, second hand
    painter->drawPolyline(GeoDataLineString() << home << approximate(home, M_PI * now.minute() / 30.0, 0.75));
    painter->drawPolyline(GeoDataLineString() << home << approximate(home, M_PI * now.hour() / 6.0, 0.5));
    painter->setPen(QPen(QBrush(Qt::red), 4.0, Qt::SolidLine, Qt::RoundCap ));
    painter->drawPolyline(GeoDataLineString() << home << approximate(home, M_PI * now.second() / 30.0, 1.0));

    return true;
}

int main(int argc, char** argv)
{
    QApplication app(argc,argv);
    MarbleWidget *mapWidget = new MarbleWidget;

    // Create and register our paint layer
    MyPaintLayer* layer = new MyPaintLayer(mapWidget);
    // Uncomment for older versions of Marble:
    // mapWidget->map()->model()->addLayer(layer);
    mapWidget->addLayer(layer);

    // Install an event handler: Pressing + will change the layer we paint at
    mapWidget->installEventFilter(layer);

    // Finish widget creation.
    mapWidget->setMapThemeId(QStringLiteral("earth/bluemarble/bluemarble.dgml"));
    // Ensure we see our rendered feature on start
    mapWidget->model()->setHome(8.4, 48.0, 1800);
    mapWidget->goHome();
    mapWidget->show();

    // Update each second to give the clock second resolution
    QTimer seconds;
    seconds.setInterval(1000);
    QObject::connect(&seconds, SIGNAL(timeout()), mapWidget, SLOT(update()));
    seconds.start();

    return app.exec();
}
