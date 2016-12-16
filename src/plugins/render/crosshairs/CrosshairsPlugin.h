//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2010 Cezar Mocan <mocancezar@gmail.com>
//

//
// This class is a crosshairs plugin.
//

#ifndef MARBLE_CROSSHAIRSPLUGIN_H
#define MARBLE_CROSSHAIRSPLUGIN_H

#include <QPixmap>


#include "RenderPlugin.h"
#include "DialogConfigurationInterface.h"

class QSvgRenderer;

namespace Ui {
    class CrosshairsConfigWidget;
}

namespace Marble
{


/**
 * @short The class that specifies the Marble layer interface of a plugin.
 *
 */

class CrosshairsPlugin : public RenderPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.CrosshairsPlugin")
    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN(CrosshairsPlugin)

 public:
    CrosshairsPlugin();

    explicit CrosshairsPlugin( const MarbleModel *marbleModel );

    ~CrosshairsPlugin() override;

    QStringList backendTypes() const override;

    QString renderPolicy() const override;

    QStringList renderPosition() const override;

    RenderType renderType() const override;

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

    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 ) override;

    QDialog *configDialog() override;

    QHash<QString,QVariant> settings() const override;

    void setSettings( const QHash<QString,QVariant> &settings ) override;

private Q_SLOTS:
   void readSettings();

   void writeSettings();

 private:
    Q_DISABLE_COPY( CrosshairsPlugin )

    bool m_isInitialized;

    QSvgRenderer *m_svgobj;
    QPixmap m_crosshairs;
    int m_themeIndex;

    QString m_theme;

    QDialog * m_configDialog;
    Ui::CrosshairsConfigWidget * m_uiConfigWidget;
};

}

#endif // MARBLE_CROSSHAIRSPLUGIN_H
