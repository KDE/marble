//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012-2016 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "FloatItemsLayer.h"

#include "AbstractFloatItem.h"
#include "GeoPainter.h"
#include "ViewportParams.h"

namespace Marble
{

FloatItemsLayer::FloatItemsLayer(QObject *parent) :
    QObject(parent),
    m_floatItems()
{
}

QStringList FloatItemsLayer::renderPosition() const
{
    return QStringList(QStringLiteral("FLOAT_ITEM"));
}

bool FloatItemsLayer::render(GeoPainter *painter,
                             ViewportParams *viewport,
                             const QString &renderPos,
                             GeoSceneLayer *layer)
{
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)

    foreach (AbstractFloatItem *item, m_floatItems) {
        if (!item->enabled()) {
            continue;
        }

        if (!item->isInitialized()) {
            item->initialize();
            emit renderPluginInitialized(item);
        }

        if (item->visible()) {
            item->paintEvent(painter, viewport);
        }
    }

    return true;
}

void FloatItemsLayer::addFloatItem(AbstractFloatItem *floatItem)
{
    Q_ASSERT(floatItem && "must not add a null float item to FloatItemsLayer");

    connect(floatItem, SIGNAL(settingsChanged(QString)),
            this,      SIGNAL(pluginSettingsChanged()));
    connect(floatItem, SIGNAL(repaintNeeded(QRegion)),
            this,      SIGNAL(repaintNeeded(QRegion)));
    connect(floatItem, SIGNAL(visibilityChanged(bool,QString)),
            this,      SLOT(updateVisibility(bool,QString)));

    m_floatItems.append( floatItem );
}

QList<AbstractFloatItem *> FloatItemsLayer::floatItems() const
{
    return m_floatItems;
}

QString FloatItemsLayer::runtimeTrace() const
{
    return QStringLiteral("Float Items: %1").arg(m_floatItems.size());
}

void FloatItemsLayer::updateVisibility(bool visible, const QString &nameId)
{
    emit visibilityChanged(nameId, visible);
}

}
