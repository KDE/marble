//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
//

//
// This class is a render plugin to display various Position Tracking info.
//

#ifndef MARBLEGPSINFO_H
#define MARBLEGPSINFO_H

#include <QtCore/QObject>
#include <QtCore/QHash>

#include "AbstractFloatItem.h"

#include "ui_GpsInfoPlugin.h"

namespace Marble
{

class GeoDataCoordinates;
class WidgetGraphicsItem;
class MarbleLocale;

/**
 * @short The class that displays Position Tracking info
 *
 */

class GpsInfo : public AbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( GpsInfo )
    
 public:
    explicit GpsInfo( const QPointF &point = QPointF( 10.5, 110 ),
                          const QSizeF &size = QSizeF( 135.0, 80.0 ) );
    ~GpsInfo();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;

    void initialize ();

    bool isInitialized () const;

    void forceRepaint();

 private slots:
    void updateLocation( GeoDataCoordinates coordinates, qreal speed);

 private:
    MarbleLocale* m_locale;
    Ui::GpsInfoPlugin m_widget;
    WidgetGraphicsItem* m_widgetItem;
};

}

#endif
