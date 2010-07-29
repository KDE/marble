//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Siddharth Srivastava <akssps011@gmail.com>
//

#ifndef ROUTINGPLUGIN_H
#define ROUTINGPLUGIN_H

#define QT_USE_FAST_CONCATENATION

#include "AbstractFloatItem.h"
#include "GeoDataCoordinates.h"

#include <QObject>

namespace Ui
{
  class RoutingItemWidget;
}

namespace Marble
{
class RoutingManager;
class RoutingModel;
class WidgetGraphicsItem;
class RoutingPlugin : public AbstractFloatItem
{
Q_OBJECT
Q_INTERFACES( Marble::RenderPluginInterface )
MARBLE_PLUGIN( RoutingPlugin )

public:
    explicit RoutingPlugin( const QPointF &point = QPointF( -10, -10 ) );

     ~RoutingPlugin ();

    QStringList backendTypes() const;

    void initialize();

    bool isInitialized() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon() const;

    bool eventFilter(QObject *object, QEvent *e);

    void showRoutingItem();

public slots:
    void setDestinationInformation( qreal, qreal );

    void setCurrentLocation( GeoDataCoordinates, qreal );

private:
    Q_DISABLE_COPY( RoutingPlugin )

    RoutingManager             *m_routingManager;
    RoutingModel               *m_routingModel;
    GeoDataCoordinates          m_currentPosition;
    qreal                       m_remainingTime;
    qreal                       m_remainingDistance;
    WidgetGraphicsItem          *m_widgetItem;
    Ui::RoutingItemWidget       *m_routingItem;


};

}
#endif // ROUTINGPLUGIN_H
