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

#ifndef MARBLE_LATLONBOXWIDGET_H
#define MARBLE_LATLONBOXWIDGET_H

#include <QtGui/QWidget>

#include "marble_export.h"

namespace Marble
{
class GeoDataLatLonBox;

class MARBLE_EXPORT LatLonBoxWidget: public QWidget
{
    Q_OBJECT

 public:
    explicit LatLonBoxWidget( QWidget * const parent = 0, Qt::WindowFlags const f = 0 );
    ~LatLonBoxWidget();
    GeoDataLatLonBox latLonBox() const;
    void setLatLonBox( GeoDataLatLonBox const & );

 Q_SIGNALS:
    void valueChanged();

 private Q_SLOTS:
    void updateLatSingleStep();
    void updateLonSingleStep();

 private:
    Q_DISABLE_COPY( LatLonBoxWidget )
    class Private;
    Private * const d;
};

}

#endif
