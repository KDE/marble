// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Henry de Valence <hdevalence@gmail.com>
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2010-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_SEARCHRUNNERMANAGER_H
#define MARBLE_SEARCHRUNNERMANAGER_H

#include "GeoDataLatLonBox.h"

#include "marble_export.h"

#include <QObject>
#include <QVector>

class QAbstractItemModel;
class QString;

namespace Marble
{

class GeoDataPlacemark;
class MarbleModel;
class SearchTask;

class MARBLE_EXPORT SearchRunnerManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param pluginManager The plugin manager that gives access to RunnerPlugins
     * @param parent Optional parent object
     */
    explicit SearchRunnerManager( const MarbleModel *marbleModel, QObject *parent = nullptr );

    ~SearchRunnerManager() override;

    /**
     * Search for placemarks matching the given search term.
     * @see findPlacemark is asynchronous with results returned using the
     * @see searchResultChanged signal.
     * @see searchPlacemark is blocking.
     * @see searchFinished signal indicates all runners are finished.
     */
    void findPlacemarks( const QString &searchTerm, const GeoDataLatLonBox &preferred = GeoDataLatLonBox() );
    QVector<GeoDataPlacemark *> searchPlacemarks( const QString &searchTerm, const GeoDataLatLonBox &preferred = GeoDataLatLonBox(), int timeout = 30000 );

Q_SIGNALS:
    /**
     * Placemarks were added to or removed from the model
     * @todo FIXME: this sounds like a duplication of QAbstractItemModel signals
     */
    void searchResultChanged( QAbstractItemModel *model );
    void searchResultChanged( const QVector<GeoDataPlacemark *> &result );

    /**
     * The search request for the given search term has finished, i.e. all
     * runners are finished and reported their results via the
     * @see searchResultChanged signal
     */
    void searchFinished( const QString &searchTerm );

    /**
     * Emitted whenever all runners are finished for the query
     */
    void placemarkSearchFinished();

private:
    Q_PRIVATE_SLOT( d, void addSearchResult( const QVector<GeoDataPlacemark *> &result ) )
    Q_PRIVATE_SLOT( d, void cleanupSearchTask( SearchTask *task ) )

    class Private;
    friend class Private;
    Private *const d;
};

}

#endif
