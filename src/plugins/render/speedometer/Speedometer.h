//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

//
// This class is a render plugin to display various Position Tracking info.
//

#ifndef MARBLESpeedometer_H
#define MARBLESpeedometer_H

#include <QtCore/QObject>
#include <QtCore/QHash>

#include "AbstractFloatItem.h"

#include "ui_Speedometer.h"

namespace Marble
{

class GeoDataCoordinates;
class WidgetGraphicsItem;
class MarbleLocale;
class MarbleWidget;

/**
 * @short The class that displays Position Tracking info
 *
 */

class Speedometer : public AbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( Speedometer )
    
 public:
    explicit Speedometer( const QPointF &point = QPointF( 10.5, 110 ),
                          const QSizeF &size = QSizeF( 135.0, 80.0 ) );
    ~Speedometer();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;

    void initialize ();

    bool isInitialized () const;

    bool eventFilter( QObject *object, QEvent *event );

    void forceRepaint();

 private slots:
    void updateLocation( GeoDataCoordinates coordinates, qreal speed);

 private:
    MarbleWidget* m_marbleWidget;
    MarbleLocale* m_locale;
    Ui::Speedometer m_widget;
    WidgetGraphicsItem* m_widgetItem;


};

}

#endif
