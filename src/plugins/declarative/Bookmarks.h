//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_BOOKMARKS_H
#define MARBLE_DECLARATIVE_BOOKMARKS_H

#include <QtCore/QObject>
#include <QtGui/QSortFilterProxyModel>

class MarbleWidget;

class BookmarksModel: public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY( int count READ count NOTIFY countChanged )

public:
    explicit BookmarksModel( QObject *parent = 0 );

    int count() const;

public Q_SLOTS:
    qreal longitude( int index );

    qreal latitude( int index );

    QString name( int index );

Q_SIGNALS:
    void countChanged();
};

class Bookmarks : public QObject
{
    Q_OBJECT

    Q_PROPERTY( BookmarksModel* model READ model NOTIFY modelChanged )

public:
    explicit Bookmarks( QObject* parent = 0 );

    void setMarbleWidget( MarbleWidget* widget );

    BookmarksModel* model();

public Q_SLOTS:
    bool isBookmark( qreal longitude, qreal latitude );

    void addBookmark( qreal longitude, qreal latitude, const QString &name, const QString &folder );

    void removeBookmark( qreal longitude, qreal latitude );

Q_SIGNALS:
    void modelChanged();

private:
    MarbleWidget* m_marbleWidget;

    BookmarksModel* m_proxyModel;
};

#endif
