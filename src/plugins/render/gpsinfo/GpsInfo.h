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

#include <QHash>

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
    Q_PLUGIN_METADATA(IID "org.kde.marble.GpsInfo")
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( GpsInfo )
    
 public:
    GpsInfo();
    explicit GpsInfo( const MarbleModel *marbleModel );
    ~GpsInfo() override;

    QStringList backendTypes() const override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon () const override;

    void initialize () override;

    bool isInitialized () const override;

    void forceRepaint();

 private Q_SLOTS:
    void updateLocation( const GeoDataCoordinates& coordinates, qreal speed);

 private:
    MarbleLocale* m_locale;
    Ui::GpsInfoPlugin m_widget;
    WidgetGraphicsItem* m_widgetItem;
};

}

#endif
