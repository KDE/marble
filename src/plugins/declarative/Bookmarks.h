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
#include <QtDeclarative/QtDeclarative>

#include "MarbleRunnerManager.h"

class QAbstractItemModel;

namespace Marble { class MarblePlacemarkModel; }

class MarbleWidget;

class Bookmarks : public QObject
{
    Q_OBJECT

public:
    explicit Bookmarks( QObject* parent = 0 );

    void setMarbleWidget( MarbleWidget* widget );

public Q_SLOTS:
    bool isBookmark( qreal longitude, qreal latitude );

    void addBookmark( qreal longitude, qreal latitude, const QString &name, const QString &folder );

    void removeBookmark( qreal longitude, qreal latitude );

private:
    MarbleWidget* m_marbleWidget;
};

#endif
