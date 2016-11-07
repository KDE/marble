//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012-2016 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_FLOATITEMSLAYER_H
#define MARBLE_FLOATITEMSLAYER_H

#include <QObject>
#include "LayerInterface.h"

#include <QList>
#include <QRegion>

namespace Marble
{

class AbstractFloatItem;
class RenderPlugin;

/**
 * @brief Layer for handling the rendering of screen-positioned, 2D float items.
 */
class FloatItemsLayer : public QObject, public LayerInterface
{
    Q_OBJECT

 public:
    explicit FloatItemsLayer(QObject *parent = 0);

    QStringList renderPosition() const override;

    bool render(GeoPainter *painter, ViewportParams *viewport,
       const QString &renderPos = "NONE", GeoSceneLayer *layer = 0) override;

    void addFloatItem(AbstractFloatItem *floatItem);

    /**
     * @brief Returns a list of all FloatItems of the layer
     * @return the list of the floatItems
     */
    QList<AbstractFloatItem *> floatItems() const;

    QString runtimeTrace() const override;

 Q_SIGNALS:
    /**
     * @brief Signal that a render item has been initialized
     */
    void renderPluginInitialized(RenderPlugin *renderPlugin);

    /**
     * This signal is emitted when the repaint of the view was requested by a plugin.
     * If available with the @p dirtyRegion which is the region the view will change in.
     * If dirtyRegion.isEmpty() returns true, the whole viewport has to be repainted.
     */
    void repaintNeeded(const QRegion &dirtyRegion = QRegion());

    void visibilityChanged(const QString &nameId, bool visible);

    void pluginSettingsChanged();

 private Q_SLOTS:
    void updateVisibility(bool visible, const QString &nameId);

 private:
    QList<AbstractFloatItem *> m_floatItems;
};

}

#endif
