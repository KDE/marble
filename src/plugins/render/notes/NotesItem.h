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
    Comment(QDateTime date, QString text, QString user, int uid);
    void setDate(const QDateTime& date){m_date = date;};
    void setText(const QString& text){m_text = text;};
    void setUser(const QString& user){m_user = user;};
    void setUid(const int uid){m_uid = uid;};
    QDateTime getDate() const {return m_date;};
    QString getText() const {return m_text;};
    QString getUser() const {return m_user;};
    int getUid() const {return m_uid;};
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

    void addLatestComment(const Comment& comment);

    void setComment(const Comment& comment);

    qreal width();

    qreal height();

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
