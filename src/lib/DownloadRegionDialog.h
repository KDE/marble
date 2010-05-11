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

#include <QtGui/QDialog>

#include "TileCoordsPyramid.h"
#include "marble_export.h"

namespace Marble
{
class AbstractScanlineTextureMapper;
class GeoDataLatLonAltBox;
class ViewportParams;

class MARBLE_EXPORT DownloadRegionDialog: public QDialog
{
    Q_OBJECT

 public:
    DownloadRegionDialog( ViewportParams const * const viewport,
                          AbstractScanlineTextureMapper const * const textureMapper,
                          QWidget * const parent = 0, Qt::WindowFlags const f = 0 );

    void setAllowedTileLevelRange( int const minimumTileLevel,
                                   int const maximumTileLevel );
    void setOriginatingTileLevel( int const tileLevel );

    TileCoordsPyramid region() const;

 public Q_SLOTS:
    void setMapTheme( QString const & );
    void setVisibleLatLonAltBox( GeoDataLatLonAltBox const & );

 Q_SIGNALS:
    /// This signal is emitted when the "Apply" button is pressed.
    void applied();

 private Q_SLOTS:
    void toggleSelectionMethod();
    void updateTilesCount();

 private:
    Q_DISABLE_COPY( DownloadRegionDialog )
    class Private;
    Private * const d;
};

}

#endif
