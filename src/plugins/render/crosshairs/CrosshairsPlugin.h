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

#include <QtCore/QObject>

#include "AbstractFloatItem.h"

#include "RenderPlugin.h"

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

class CrosshairsPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN(CrosshairsPlugin)

 public:
    CrosshairsPlugin();

    ~CrosshairsPlugin();

    QStringList backendTypes() const;

    QString renderPolicy() const;

    QStringList renderPosition() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;

    void initialize ();

    bool isInitialized () const;

    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );

    QDialog *configDialog() const;

    QHash<QString,QVariant> settings() const;

    void setSettings( QHash<QString,QVariant> settings );

private Q_SLOTS:
   void readSettings() const;

   void writeSettings();

 private:
    Q_DISABLE_COPY( CrosshairsPlugin )

    bool m_isInitialized;

    mutable QSvgRenderer *m_svgobj;
    mutable QPixmap m_crosshairs;

    QHash<QString,QVariant> m_settings;
    /** @todo: Refactor plugin interface to have configDialog() non-const */
    mutable QDialog * m_configDialog;
    mutable Ui::CrosshairsConfigWidget * m_uiConfigWidget;
};

}

#endif // MARBLE_CROSSHAIRSPLUGIN_H
