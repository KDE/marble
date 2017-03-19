//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Daniel Marth <danielmarth@gmx.at>
// Copyright 2012        Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef QT_POSITIONING_POSITION_PROVIDER_PLUGIN_H
#define QT_POSITIONING_POSITION_PROVIDER_PLUGIN_H

#include "PositionProviderPlugin.h"

#include <QGeoPositionInfo>

namespace Marble
{

class QtPositioningPositionProviderPluginPrivate;

class QtPositioningPositionProviderPlugin: public PositionProviderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.QtPositioningPositionProviderPlugin")
    Q_INTERFACES( Marble::PositionProviderPluginInterface )

public:
    QtPositioningPositionProviderPlugin();
    ~QtPositioningPositionProviderPlugin() override;

    // Implementing PluginInterface
    QString name() const override;
    QString nameId() const override;
    QString guiString() const override;
    QString version() const override;
    QString description() const override;
    QString copyrightYears() const override;
    QVector<PluginAuthor> pluginAuthors() const override;
    QIcon icon() const override;
    void initialize() override;
    bool isInitialized() const override;
    qreal speed() const override;
    qreal direction() const override;
    QDateTime timestamp() const override;

    // Implementing PositionProviderPlugin
    PositionProviderPlugin * newInstance() const override;

    // Implementing PositionProviderPluginInterface
    PositionProviderStatus status() const override;
    GeoDataCoordinates position() const override;
    GeoDataAccuracy accuracy() const override;

private Q_SLOTS:
    /** Regular (each second) position and status update */
    void update();
    void update(const QGeoPositionInfo& position);

private:
    QtPositioningPositionProviderPluginPrivate* const d;

};

}

#endif // QT_POSITION_PROVIDER_PLUGIN_H
