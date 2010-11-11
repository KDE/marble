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

class MapThemeManager : QObject
{
%TypeHeaderCode
#include <MapThemeManager.h>
%End

public:
    explicit                MapThemeManager (QObject* parent /TransferThis/ = 0);
    QStandardItemModel*     mapThemeModel ();
    void                    updateMapThemeModel ();
    void                    directoryChanged (const QString& path);
    void                    fileChanged (const QString& path);

    ~MapThemeManager ();
//ig    static Marble::GeoSceneDocument*  loadMapTheme (const QString& mapThemeStringID);
//ig    QList<const GeoSceneDocument*>  mapThemes () const;
signals:
    void                    themesChanged ();
};
// MapThemeManager

};
// Marble


