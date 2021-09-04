// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_LABELGRAPHICSITEMPRIVATE_H
#define MARBLE_LABELGRAPHICSITEMPRIVATE_H

#include "FrameGraphicsItem_p.h"

#include <QString>
#include <QSizeF>
#include <QImage>
#include <QIcon>

class QFont;

namespace Marble
{

class LabelGraphicsItem;

class LabelGraphicsItemPrivate : public FrameGraphicsItemPrivate
{
 public:
    LabelGraphicsItemPrivate(LabelGraphicsItem *labelGraphicsItem,
                             MarbleGraphicsItem *parent);

    // TODO: This has to go up to MarbleGraphicsItem
    static QFont font();

    QString m_text;
    QImage m_image;
    QIcon m_icon;

    QSizeF m_minimumSize;
};

} // namespace Marble

#endif
