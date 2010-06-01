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

class MarbleControlBox : QWidget
{
%TypeHeaderCode
#include <MarbleControlBox.h>
%End

public:
    explicit                MarbleControlBox (QWidget* parent /TransferThis/ = 0);
    void                    addMarbleWidget (Marble::MarbleWidget* widget);
    int                     minimumZoom () const;
    void                    setMapThemeModel (QStandardItemModel* mapThemeModel);
    void                    updateCelestialModel ();

signals:
    void                    goHome ();
    void                    zoomIn ();
    void                    zoomOut ();
    void                    zoomChanged (int zoom);
    void                    moveLeft ();
    void                    moveRight ();
    void                    moveUp ();
    void                    moveDown ();
    void                    centerOn (const QModelIndex&, bool);
    void                    selectMapTheme (const QString&);
    void                    projectionSelected (Marble::Projection /Constrained/);
    void                    gpsInputDisabled (bool);
    void                    gpsPositionChanged (qreal lat, qreal lon);

public:
    void                    selectTheme (const QString&);
    void                    selectProjection (Marble::Projection projection);
// FIXME PlacemarkModel header isn't installed
//ig    void                    setLocations (MarblePlacemarkModel* locations);
    void                    changeZoom (int zoom);
    void                    enableFileViewActions ();
    void                    setNavigationTabShown (bool show);
    void                    setLegendTabShown (bool show);
    void                    setMapViewTabShown (bool show);
    void                    setCurrentLocationTabShown (bool show);
    void                    setFileViewTabShown (bool show);
    void                    updateMapThemeView ();
    void                    projectionSelected (int projectionIndex);

protected:
    void                    resizeEvent (QResizeEvent*);

public:
    ~MarbleControlBox ();
    void                    selectCurrentMapTheme (const QString&);
    void                    setWorkOffline (bool offline);
    void                    receiveGpsCoordinates (const Marble::GeoDataCoordinates& in, qreal speed);
};
// MarbleControlBox

};
// Marble


