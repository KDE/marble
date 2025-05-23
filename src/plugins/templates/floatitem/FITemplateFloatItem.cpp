// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>
//

// Local
#include "FITemplateFloatItem.h"

// Qt
#include <QColor>
#include <QRect>
#include <QSvgRenderer>

// Marble
#include "GeoDataCoordinates.h"
#include "GeoPainter.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "ViewportParams.h"

namespace Marble
{

FITemplateFloatItem::FITemplateFloatItem(const QPointF &point, const QSizeF &size)
    : AbstractFloatItem(point, size)
    , m_compass()
    , m_polarity(0)
{
    m_svgobj = new QSvgRenderer(MarbleDirs::path(QStringLiteral("svg/compass.svg")), this);
}

FITemplateFloatItem::~FITemplateFloatItem()
{
    delete m_svgobj;
}

QStringList FITemplateFloatItem::backendTypes() const
{
    return QStringList(QStringLiteral("FITemplate"));
}

QString FITemplateFloatItem::name() const
{
    return tr("FITemplate");
}

QString FITemplateFloatItem::guiString() const
{
    return tr("Float Item &Template");
}

QString FITemplateFloatItem::nameId() const
{
    return QStringLiteral("floatitemtemplate");
}

QString FITemplateFloatItem::description() const
{
    return tr("This is a template class for float items.");
}

QIcon FITemplateFloatItem::icon() const
{
    return QIcon();
}

void FITemplateFloatItem::initialize()
{
    // Initialize your float item here
}

bool FITemplateFloatItem::isInitialized() const
{
    // Return whether your float item is initialized here.
    return true;
}

QPainterPath FITemplateFloatItem::backgroundShape() const
{
    // Return a QPainterPath here that contains the outer shape of
    // your float item.
}

bool FITemplateFloatItem::needsUpdate(ViewportParams *viewport)
{
    // Return whether your float item needs to be redrawn.
}

bool FITemplateFloatItem::renderFloatItem(GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer *layer)
{
    // Here you should render your float item
}

}

#include "moc_FITemplateFloatItem.cpp"
