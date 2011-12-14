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

#include <QtCore/QObject>
#include <QtDeclarative/QtDeclarative>

#include "MarbleRunnerManager.h"

class QAbstractItemModel;

namespace Marble
{

class MarblePlacemarkModel;

namespace Declarative
{

class MarbleWidget;

class Search : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QDeclarativeComponent* placemarkDelegate READ placemarkDelegate WRITE setPlacemarkDelegate NOTIFY placemarkDelegateChanged )

public:
    explicit Search( QObject* parent = 0 );

    void setMarbleWidget( Marble::Declarative::MarbleWidget* widget );

    void setDelegateParent( QGraphicsItem* parent );

    QDeclarativeComponent* placemarkDelegate();

    void setPlacemarkDelegate( QDeclarativeComponent* delegate );

Q_SIGNALS:
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
    Marble::Declarative::MarbleWidget* m_marbleWidget;

    /** Wrapped Marble runner manager */
    Marble::MarbleRunnerManager *m_runnerManager;

    /** Search result */
    MarblePlacemarkModel *m_searchResult;

    QDeclarativeComponent* m_placemarkDelegate;

    QGraphicsItem* m_delegateParent;

    QMap<int,QDeclarativeItem*> m_placemarks;
};

}

}

#endif
