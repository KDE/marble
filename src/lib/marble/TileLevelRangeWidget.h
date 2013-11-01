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

#ifndef MARBLE_TILELEVELRANGEWIDGET_H
#define MARBLE_TILELEVELRANGEWIDGET_H

#include <QWidget>

#include "marble_export.h"

namespace Marble
{

class MARBLE_EXPORT TileLevelRangeWidget: public QWidget
{
    Q_OBJECT

 public:
    explicit TileLevelRangeWidget( QWidget * const parent = 0, Qt::WindowFlags const f = 0 );
    ~TileLevelRangeWidget();

    virtual QSize sizeHint() const;

    void setAllowedLevelRange( int const minimumLevel, int const maximumLevel );
    void setDefaultLevel( int const );

    int topLevel() const;
    int bottomLevel() const;

 Q_SIGNALS:
    void topLevelChanged( int );
    void bottomLevelChanged( int );

 private Q_SLOTS:
    void setMaximumTopLevel( int const );
    void setMinimumBottomLevel( int const );

 private:
    Q_DISABLE_COPY( TileLevelRangeWidget )
    class Private;
    Private * const d;
};

}

#endif
