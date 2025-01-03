// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008-2009 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
// SPDX-FileCopyrightText: 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

// Own
#include "MarbleSplashLayer.h"

#include "GeoPainter.h"
#include "MarbleDirs.h"
#include "RenderState.h"
#include "ViewportParams.h"

#include <QPixmap>

namespace Marble
{

MarbleSplashLayer::MarbleSplashLayer() = default;

QStringList MarbleSplashLayer::renderPosition() const
{
    return QStringList(QStringLiteral("SURFACE"));
}

bool MarbleSplashLayer::render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer)
{
    Q_UNUSED(renderPos);
    Q_UNUSED(layer);

    painter->save();

    QPixmap logoPixmap(MarbleDirs::path(QStringLiteral("svg/marble-logo-inverted-72dpi.png")));

    if (logoPixmap.width() > viewport->width() * 0.7 || logoPixmap.height() > viewport->height() * 0.7) {
        logoPixmap = logoPixmap.scaled(QSize(viewport->width(), viewport->height()) * 0.7, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QPoint logoPosition((viewport->width() - logoPixmap.width()) / 2, (viewport->height() - logoPixmap.height()) / 2);
    painter->drawPixmap(logoPosition, logoPixmap);

    QString message; // "Please assign a map theme!";

    painter->setPen(Qt::white);

    int yTop = logoPosition.y() + logoPixmap.height() + 10;
    QRect textRect(0, yTop, viewport->width(), viewport->height() - yTop);
    painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, message);

    painter->restore();

    return true;
}

RenderState MarbleSplashLayer::renderState() const
{
    return RenderState(QStringLiteral("Splash Screen"));
}

}
