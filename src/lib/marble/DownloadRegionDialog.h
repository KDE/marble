// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library. If not, see <http://www.gnu.org/licenses/>.

#ifndef MARBLE_DOWNLOADREGIONDIALOG_H
#define MARBLE_DOWNLOADREGIONDIALOG_H

#include <QDialog>
#include <QVector>

#include "marble_export.h"

namespace Marble
{
class GeoDataLatLonAltBox;
class TileCoordsPyramid;
class MarbleWidget;

class MARBLE_EXPORT DownloadRegionDialog: public QDialog
{
    Q_OBJECT

 public:
    enum SelectionMethod { VisibleRegionMethod, SpecifiedRegionMethod, RouteDownloadMethod };

    explicit DownloadRegionDialog( MarbleWidget *const widget, QWidget * const parent = 0,
                                   Qt::WindowFlags const f = 0 );
    ~DownloadRegionDialog() override;
    void setAllowedTileLevelRange( int const minimumTileLevel,
                                   int const maximumTileLevel );
    void setVisibleTileLevel( int const tileLevel );
    void setSelectionMethod( SelectionMethod const );

    QVector<TileCoordsPyramid> region() const;

 public Q_SLOTS:
    void setSpecifiedLatLonAltBox( GeoDataLatLonAltBox const & );
    void setVisibleLatLonAltBox( GeoDataLatLonAltBox const & );
    void updateTextureLayer();

 Q_SIGNALS:
    /// This signal is emitted when the "Apply" button is pressed.
    void applied();
    /// This signal is emitted when the dialog receives a QHideEvent.
    void hidden();
    /// This signal is emitted when the dialog receives a QShowEvent
    void shown();

 protected:
    void hideEvent( QHideEvent * event ) override;
    void showEvent( QShowEvent * event ) override;

 private Q_SLOTS:
    void toggleSelectionMethod();
    void updateTilesCount();

    /// This slot is called upon to update the route download UI when a route exists
    void updateRouteDialog();
    /// This slot sets the unit of the offset(m or km) in the spinbox
    void setOffsetUnit();

 private:
    Q_DISABLE_COPY( DownloadRegionDialog )
    class Private;
    Private * const d;

};

}

#endif
