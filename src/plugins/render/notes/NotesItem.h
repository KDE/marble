//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Spencer Brown <spencerbrown991@gmail.com>
//

#ifndef NOTESITEM_H
#define NOTESITEM_H

#include "AbstractDataPluginItem.h"
#include <QPixmap>
#include <QLabel>
#include <QDateTime>

class Comment
{
public:
    Comment();
    Comment(const QDateTime &date, const QString &text, const QString &user, int uid);
    QDateTime date() const;
    QString text() const;
    QString user() const;
    int uid() const;

private:
    QDateTime m_date;
    QString m_text;
    QString m_user;
    int m_uid;
};

namespace Marble
{

class NotesItem : public AbstractDataPluginItem
{
    Q_OBJECT

public:
    explicit NotesItem(QObject* parent);

    ~NotesItem() override;

    bool initialized() const override;

    void paint(QPainter *painter) override;

    bool operator<(const AbstractDataPluginItem *other) const override;

    void setAuthor(const QString &author);

    void setDateCreated(const QDateTime& dateCreated);

    void setNoteStatus(const QString& noteStatus);

    void setDateClosed(const QDateTime& dataClosed);

    void addComment(const Comment& comment);

private:
    QPixmap m_pixmap_open;
    QPixmap m_pixmap_closed;
    QVector<Comment> m_commentsList;
    QDateTime m_dateCreated;
    QString m_noteStatus;
    QDateTime m_dateClosed;
    QString m_labelText;

    static const QFont s_font;
    static const int s_labelOutlineWidth;
};
}
#endif // NOTESITEM_H
