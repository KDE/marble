//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_SEARCH_H
#define MARBLE_DECLARATIVE_SEARCH_H

#include <QObject>
#include <QtQml/qqml.h>
#include <QQuickItem>

class QAbstractItemModel;

namespace Marble {
    class MarblePlacemarkModel;
    class SearchRunnerManager;
}

class MarbleWidget;

class Search : public QObject
{
    Q_OBJECT

    Q_PROPERTY( MarbleWidget* map READ map WRITE setMap NOTIFY mapChanged )
    Q_PROPERTY( QQmlComponent* placemarkDelegate READ placemarkDelegate WRITE setPlacemarkDelegate NOTIFY placemarkDelegateChanged )

public:
    explicit Search( QObject* parent = 0 );

    MarbleWidget *map();

    void setMap( MarbleWidget* widget );

    QQmlComponent* placemarkDelegate();

    void setPlacemarkDelegate( QQmlComponent* delegate );

Q_SIGNALS:
    void mapChanged();

    /**
     * The last search triggered by search() is finished and can be
     * retrieved using @see searchResult
     */
    void searchFinished();

    void placemarkDelegateChanged();

public Q_SLOTS:
    void find( const QString &searchTerm );

    QObject* searchResultModel();

private Q_SLOTS:
    /** Search progress update */
    void updateSearchModel( QAbstractItemModel *model );

    void updatePlacemarks();

    void handleSearchResult();

private:
    MarbleWidget* m_marbleWidget;

    /** Wrapped Marble runner manager */
    Marble::SearchRunnerManager *m_runnerManager;

    /** Search result */
    Marble::MarblePlacemarkModel *m_searchResult;

    QQmlComponent* m_placemarkDelegate;

    QMap<int,QQuickItem*> m_placemarks;
};

#endif
