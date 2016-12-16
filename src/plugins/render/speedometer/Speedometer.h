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
    ~Speedometer() override;

    QStringList backendTypes() const override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QString copyrightYears() const override;

    QIcon icon () const override;

    void initialize () override;

    bool isInitialized () const override;

 private Q_SLOTS:
    void updateLocation( const GeoDataCoordinates& coordinates, qreal speed );

 private:
    MarbleLocale* m_locale;
    Ui::Speedometer m_widget;
    WidgetGraphicsItem* m_widgetItem;
};

}

#endif
