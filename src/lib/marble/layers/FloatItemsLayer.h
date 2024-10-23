// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012-2016 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_FLOATITEMSLAYER_H
#define MARBLE_FLOATITEMSLAYER_H

#include "LayerInterface.h"
#include <QObject>

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
    explicit FloatItemsLayer(QObject *parent = nullptr);

    QStringList renderPosition() const override;

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos = QStringLiteral("NONE"), GeoSceneLayer *layer = nullptr) override;

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
