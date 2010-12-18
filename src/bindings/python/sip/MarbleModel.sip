//
// Copyright 2008 Simon Edwards <simon@simonzone.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.
//

namespace Marble
{

class MarbleModel : QObject
{
%TypeHeaderCode
#include <MarbleModel.h>
%End

public:
    explicit                MarbleModel (QObject* parent /TransferThis/);
    QAbstractItemModel*     placemarkModel () const;
    QItemSelectionModel*    placemarkSelectionModel () const;
    QString                 mapThemeId () const;
    void                    addPlacemarkFile (const QString& filename);
    void                    addPlacemarkData (const QString& data, const QString& key = "data");
    void                    removePlacemarkKey (const QString& key);
//ig    FileViewModel*          fileViewModel () const;
    qreal                   planetRadius () const;
    QString                 planetName () const;
    Marble::SunLocator*     sunLocator () const;
//ig    quint64                 volatileTileCacheLimit () const;
    Marble::Planet*         planet () const;
    void                    clearPersistentTileCache ();

signals:
    void                    creatingTilesStart (Marble::TileCreator*, const QString& name, const QString& description);
    void                    themeChanged (QString mapTheme);
    void                    modelChanged ();

public:
    virtual ~MarbleModel ();
//ig    Marble::GeoSceneDocument*  mapTheme () const;
//ig    HttpDownloadManager*    downloadManager () const;
    void                    openGpxFile (const QString& filename);
    QAbstractItemModel*     treeModel () const;
    void                    home (qreal& lon, qreal& lat, int& zoom);
    void                    setHome (qreal lon, qreal lat, int zoom = 1050);
    void                    setHome (const Marble::GeoDataCoordinates& homePoint, int zoom = 1050);
    void                    addGeoDataFile (const QString& filename);
    void                    addGeoDataString (const QString& data, const QString& key = "data");
    void                    removeGeoData (const QString& key);
    Marble::PositionTracking*  positionTracking () const;
//ig    MarbleClock*            clock () const;
    quint64                 persistentTileCacheLimit () const;
//ig    PluginManager*          pluginManager () const;
//ig    Marble::RoutingManager*  routingManager ();
    void                    setClockDateTime (const QDateTime& datetime);
    QDateTime               clockDateTime () const;
    int                     clockSpeed () const;
    void                    setClockSpeed (int speed);
    void                    setClockTimezone (int timeInSec);
    int                     clockTimezone () const;
//ig    BookmarkManager*        bookmarkManager () const;
    QTextDocument*          legend ();
    void                    setLegend (QTextDocument* document);
    void                    setPersistentTileCacheLimit (quint64 kiloBytes);
    QAbstractItemModel*     popSortModel () const;
//ig    void                    setMapTheme (GeoSceneDocument* mapTheme);
    Marble::MapThemeManager*  mapThemeManager () const;
    Marble::MarbleDataFacade*  dataFacade () const;
};
// MarbleModel

};
// Marble


