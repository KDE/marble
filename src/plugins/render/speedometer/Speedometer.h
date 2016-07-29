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

#include "AbstractFloatItem.h"

#include "ui_Speedometer.h"

namespace Marble
{

class GeoDataCoordinates;
class WidgetGraphicsItem;
class MarbleLocale;

/**
 * @short The class that displays Position Tracking info
 *
 */

class Speedometer : public AbstractFloatItem
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.Speedometer")
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( Speedometer )
    
 public:
    Speedometer();
    explicit Speedometer( const MarbleModel *marbleModel );
    ~Speedometer();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QVector<PluginAuthor> pluginAuthors() const override;

    QString copyrightYears() const;

    QIcon icon () const;

    void initialize ();

    bool isInitialized () const;

 private Q_SLOTS:
    void updateLocation( const GeoDataCoordinates& coordinates, qreal speed );

 private:
    MarbleLocale* m_locale;
    Ui::Speedometer m_widget;
    WidgetGraphicsItem* m_widgetItem;
};

}

#endif
