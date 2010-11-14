//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_RUNNERMANAGER_H
#define MARBLE_DECLARATIVE_RUNNERMANAGER_H

#include "Placemark.h"

#include "MarbleRunnerManager.h"
#include "MarblePlacemarkModel.h"

#include <QtCore/QObject>

namespace Marble
{
namespace Declarative
{

/**
  * Wraps Marble::MarbleRunnerManager to execute searches for placemarks
  * (cities, addresses, other places)
  */
class MarbleRunnerManager : public QObject
{
    Q_OBJECT

public:
    /** Constructor */
    explicit MarbleRunnerManager( QObject *parent = 0 );

Q_SIGNALS:
    /**
     * The last search triggered by search() is finished and can be
     * retrieved using @see searchResult
     */
    void searchFinished();

public Q_SLOTS:
    /**
      * Execute a search, cancelling any running searches. Run asynchronously,
      * results will be available in @see searchResult after the search is
      * finished. The latter is announced via @see searchFinished
      */
    void search( const QString &term );

    /**
      * Provides access to the search result. Each item in the list is an
      * instance of a Placemark
      */
    QList<QObject*> searchResult() const;

private Q_SLOTS:
    /** Search progress update */
    void searchModelChanged( MarblePlacemarkModel *model );

private:
    /** Wrapped Marble runner manager */
    Marble::MarbleRunnerManager *m_runnerManager;

    /** Search result */
    MarblePlacemarkModel *m_searchResult;
};

}
}

#endif // MARBLE_DECLARATIVE_RUNNERMANAGER_H
