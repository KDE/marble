// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Spencer Brown <spencerbrown991@gmail.com>
//

#include "NotesItem.h"
#include "MarbleDirs.h"

#include <QPainter>
#include <QRect>

using namespace Marble;

const QFont NotesItem::s_font = QFont(QStringLiteral("Sans Serif"), 10);
const int NotesItem::s_labelOutlineWidth = 5;

NotesItem::NotesItem(QObject *parent)
    : AbstractDataPluginItem(parent),
      m_pixmap_open(QPixmap(MarbleDirs::path("bitmaps/notes_open.png"))),
      m_pixmap_closed(QPixmap(MarbleDirs::path("bitmaps/notes_closed.png")))
{
    setSize(m_pixmap_open.size());
    setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    setCacheMode(ItemCoordinateCache);
}

NotesItem::~NotesItem()
{
}

bool NotesItem::initialized() const
{
    return !id().isEmpty();
}

bool NotesItem::operator<(const AbstractDataPluginItem *other) const
{
    return this->id() < other->id();
}

void NotesItem::paint(QPainter *painter)
{
    painter->save();

    painter->setFont(s_font);
    const int fontAscent = painter->fontMetrics().ascent();
    QPen outlinepen(Qt::white);
    outlinepen.setWidthF(s_labelOutlineWidth);
    QBrush  outlinebrush(Qt::black);

    const QPointF baseline(s_labelOutlineWidth / 2.0, fontAscent);

    QPainterPath outlinepath;
    outlinepath.addText(baseline, painter->font(), m_labelText);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(outlinepen);
    painter->setBrush(outlinebrush);
    painter->drawPath(outlinepath);
    painter->setPen(Qt::NoPen);
    painter->drawPath(outlinepath);
    painter->setRenderHint(QPainter::Antialiasing, false);

    int const y = qMax(0, int(size().width() - m_pixmap_open.width()) / 2);

    //The two pixmaps have the same dimensions, so all the logic for one works for the other
    QPixmap const & pixmap = m_noteStatus == "closed" ? m_pixmap_closed : m_pixmap_open;
    painter->drawPixmap(y, 2 + painter->fontMetrics().height(), pixmap);

    painter->restore();
}

void NotesItem::setDateCreated(const QDateTime& dateCreated)
{
    m_dateCreated = dateCreated;
}

void NotesItem::setDateClosed(const QDateTime& dateClosed)
{
    m_dateClosed = dateClosed;
}

void NotesItem::setNoteStatus(const QString& noteStatus)
{
    m_noteStatus = noteStatus;
}

void NotesItem::addComment(const Comment& comment)
{
    m_commentsList.push_back(comment);
    std::sort(m_commentsList.begin(), m_commentsList.end(), [](const Comment & a, const Comment & b) {
        return a.date() > b.date();
    });

    QStringList toolTip;
    for (auto const &entry: m_commentsList) {
        QString const date = entry.date().toString(Qt::SystemLocaleShortDate);
        QString const user = entry.user().isEmpty() ? tr("anonymous", "The author name is not known") : entry.user();
        toolTip << QStringLiteral("%1\n--%2, %3").arg(entry.text().trimmed()).arg(user).arg(date);
    }
    setToolTip(toolTip.join(QStringLiteral("\n\n")));
    QFontMetrics fontmet(s_font);
    m_labelText = fontmet.elidedText(m_commentsList.front().text(), Qt::ElideRight, 125);
    auto const width = qMax(fontmet.width(m_labelText), m_pixmap_open.width());
    setSize(QSizeF(width, fontmet.height() + 2 + m_pixmap_open.height()));
}

Comment::Comment() : m_uid(0)
{
}

Comment::Comment(const QDateTime &date, const QString &text, const QString &user, int uid)
    : m_date(date)
    , m_text(text)
    , m_user(user)
{
    m_uid = uid;
}

QDateTime Comment::date() const
{
    return m_date;
}

QString Comment::text() const
{
    return m_text;
}

QString Comment::user() const
{
    return m_user;
}

int Comment::uid() const
{
    return m_uid;
}

#include "moc_NotesItem.cpp"
