//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2011      Jens-Michael Hoffmann <jmho@c-xx.com>
//

#ifndef MARBLE_ATMOSPHEREPLUGIN_H
#define MARBLE_ATMOSPHEREPLUGIN_H

#include "RenderPlugin.h"

#include <QPixmap>

namespace Marble
{

class AtmospherePlugin : public RenderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.AtmospherePlugin")
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( AtmospherePlugin )

public:
    AtmospherePlugin();

    explicit AtmospherePlugin( const MarbleModel *marbleModel );

    QStringList backendTypes() const;

    QString renderPolicy() const;

    QStringList renderPosition() const;

    virtual RenderType renderType() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QIcon icon() const;

    QString copyrightYears() const;

    QVector<PluginAuthor> pluginAuthors() const override;

    qreal zValue() const;

    void initialize();

    bool isInitialized() const;

    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );
    void repaintPixmap(const ViewportParams *viewParams);

public Q_SLOTS:
    void updateTheme();

private:
    QPixmap m_renderPixmap;
    QColor m_renderColor;
    int m_renderRadius;
};

}

#endif // MARBLE_ATMOSPHEREPLUGIN_H
