// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Valery Kharitonov <kharvd@gmail.com>
//

// Self
#include "PostalCodeItem.h"

// Marble
#include "ViewportParams.h"

// Qt
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>

using namespace Marble;

const QFont PostalCodeItem::s_font = QFont(QStringLiteral("Sans Serif"), 10, QFont::Bold);
const int PostalCodeItem::s_labelOutlineWidth = 5;

PostalCodeItem::PostalCodeItem(QObject *parent)
    : AbstractDataPluginItem(parent)
{
    setSize(QSize(0, 0));
    setCacheMode(ItemCoordinateCache);
}

PostalCodeItem::~PostalCodeItem() = default;

bool PostalCodeItem::initialized() const
{
    return !m_text.isEmpty();
}

bool PostalCodeItem::operator<(const AbstractDataPluginItem *other) const
{
    return this->id() < other->id();
}

QString PostalCodeItem::text() const
{
    return m_text;
}

void PostalCodeItem::setText(const QString &text)
{
    QFontMetrics metrics(s_font);
    setSize(metrics.size(0, text) + QSize(10, 10));
    m_text = text;
}

void PostalCodeItem::paint(QPainter *painter)
{
    painter->save();

    const int fontAscent = QFontMetrics(s_font).ascent();

    QPen outlinepen(Qt::white);
    outlinepen.setWidthF(s_labelOutlineWidth);
    QBrush outlinebrush(Qt::black);

    const QPointF baseline(s_labelOutlineWidth / 2.0, fontAscent);

    QPainterPath outlinepath;
    outlinepath.addText(baseline, s_font, m_text);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(outlinepen);
    painter->setBrush(outlinebrush);
    painter->drawPath(outlinepath);
    painter->setPen(Qt::NoPen);
    painter->drawPath(outlinepath);
    painter->setRenderHint(QPainter::Antialiasing, false);

    painter->restore();
}

#include "moc_PostalCodeItem.cpp"
