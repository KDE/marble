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


    void                    selectTheme (const QString&);
// FIXME PlacemarkModel header isn't installed
    void                    setNavigationTabShown (bool show);
    void                    setLegendTabShown (bool show);
    void                    setMapViewTabShown (bool show);
    void                    setCurrentLocationTabShown (bool show);
    void                    setFileViewTabShown (bool show);


    ~MarbleControlBox ();
    void                    setWorkOffline (bool offline);
    void                    setRoutingTabShown (bool show);
signals:
    void                    showMapWizard ();
    void                    showUploadDialog ();
public:
    Marble::CurrentLocationWidget*  currentLocationWidget ();
    void                    setMarbleWidget (Marble::MarbleWidget* widget);
    void                    search (const QString& searchTerm);
signals:
    void                    searchFinished ();
};
// MarbleControlBox

};
// Marble


