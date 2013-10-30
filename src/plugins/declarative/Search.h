//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_SEARCH_H
#define MARBLE_DECLARATIVE_SEARCH_H

#include <QObject>
#if QT_VERSION < 0x050000
  #include <QtDeclarative/qdeclarative.h>
  #include <QDeclarativeComponent>
  #include <QDeclarativeItem>
#else
  #include <QtQml/qqml.h>
  #include <QQuickItem>
#endif

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
#if QT_VERSION < 0x050000
    Q_PROPERTY( QDeclarativeComponent* placemarkDelegate READ placemarkDelegate WRITE setPlacemarkDelegate NOTIFY placemarkDelegateChanged )
#else
    Q_PROPERTY( QQmlComponent* placemarkDelegate READ placemarkDelegate WRITE setPlacemarkDelegate NOTIFY placemarkDelegateChanged )
#endif

public:
    explicit Search( QObject* parent = 0 );

    MarbleWidget *map();

    void setMap( MarbleWidget* widget );

#if QT_VERSION < 0x050000
    QDeclarativeComponent* placemarkDelegate();

    void setPlacemarkDelegate( QDeclarativeComponent* delegate );
#else
    QQmlComponent* placemarkDelegate();

    void setPlacemarkDelegate( QQmlComponent* delegate );
#endif

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

#if QT_VERSION < 0x050000
    QDeclarativeComponent* m_placemarkDelegate;

    QMap<int,QDeclarativeItem*> m_placemarks;
#else
    QQmlComponent* m_placemarkDelegate;

    QMap<int,QQuickItem*> m_placemarks;
#endif
};

#endif
