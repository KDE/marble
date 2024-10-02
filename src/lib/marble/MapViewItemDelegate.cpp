// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2004-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
// SPDX-FileCopyrightText: 2011-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

// Self
#include "MapViewItemDelegate.h"

// Qt
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QListView>
#include <QModelIndex>
#include <QPainter>
#include <QSettings>
#include <QTextDocument>

namespace Marble
{
MapViewItemDelegate::MapViewItemDelegate(QListView *view)
    : m_view(view)
    , m_bookmarkIcon(QStringLiteral(":/icons/bookmarks.png"))
{
    // nothing to do
}

void MapViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem styleOption = option;
    initStyleOption(&styleOption, index);
    styleOption.text = QString();
    styleOption.icon = QIcon();

    bool const selected = styleOption.state & QStyle::State_Selected;
    bool const active = styleOption.state & QStyle::State_Active;
    bool const hover = styleOption.state & QStyle::State_MouseOver;
    QPalette::ColorGroup const colorGroup = active ? QPalette::Active : QPalette::Inactive;
    if (selected || hover) {
        styleOption.features &= ~QStyleOptionViewItem::Alternate;
        QPalette::ColorRole colorRole = selected ? QPalette::Highlight : QPalette::Midlight;
        painter->fillRect(styleOption.rect, styleOption.palette.color(colorGroup, colorRole));
    }
    QStyle *style = styleOption.widget ? styleOption.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &styleOption, painter, styleOption.widget);

    QRect const rect = styleOption.rect;
    QSize const iconSize = styleOption.decorationSize;
    QRect const iconRect(rect.topLeft(), iconSize);
    auto const icon = index.data(Qt::DecorationRole).value<QIcon>();
    painter->drawPixmap(iconRect, icon.pixmap(iconSize));

    int const padding = 5;
    QString const name = index.data().toString();
    const bool isFavorite = QSettings().contains(QLatin1StringView("Favorites/") + name);
    QSize const bookmarkSize(16, 16);
    QRect bookmarkRect(iconRect.bottomRight(), bookmarkSize);
    bookmarkRect.translate(QPoint(-bookmarkSize.width() - padding, -bookmarkSize.height() - padding));
    QIcon::Mode mode = isFavorite ? QIcon::Normal : QIcon::Disabled;
    painter->drawPixmap(bookmarkRect, m_bookmarkIcon.pixmap(bookmarkSize, mode));

    QTextDocument document;
    document.setTextWidth(rect.width() - iconSize.width() - padding);
    document.setDefaultFont(styleOption.font);
    document.setHtml(text(index));

    QRect textRect = QRect(iconRect.topRight(), QSize(document.textWidth() - padding, rect.height() - padding));
    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    QAbstractTextDocumentLayout::PaintContext paintContext;
    paintContext.palette = styleOption.palette;
    QPalette::ColorRole const role = selected && active ? QPalette::HighlightedText : QPalette::Text;
    paintContext.palette.setColor(QPalette::Text, styleOption.palette.color(colorGroup, role));
    document.documentLayout()->draw(painter, paintContext);
    painter->restore();
}

QSize MapViewItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 0) {
        QSize const iconSize = option.decorationSize;
        QTextDocument doc;
        doc.setDefaultFont(option.font);
        doc.setTextWidth(m_view->width() - iconSize.width() - 10);
        doc.setHtml(text(index));
        return QSize(iconSize.width() + doc.size().width(), iconSize.height());
    }

    return {};
}

QString MapViewItemDelegate::text(const QModelIndex &index)
{
    QString const title = index.data().toString();
    QString const description = index.data(Qt::UserRole + 2).toString();
    return QStringLiteral("<p><b>%1</b></p>%2").arg(title, description);
}

}

#include "moc_MapViewItemDelegate.cpp"
