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

class MarbleRenderPlugin : QObject, Marble::MarbleRenderPluginInterface /Abstract/
{
%TypeHeaderCode
#include <marble/MarbleRenderPlugin.h>
%End

public:
    explicit                MarbleRenderPlugin ();
    Marble::MarbleDataFacade*  dataFacade () const;
    void                    setDataFacade (Marble::MarbleDataFacade*);
    QAction*                action () const;
    QStandardItem*          item () const;
    void                    applyItemState ();
    void                    retrieveItemState ();
    bool                    enabled () const;
    bool                    visible () const;
    void                    setEnabled (bool enabled);
    void                    setVisible (bool visible);

signals:
    void                    valueChanged (QString nameId, bool visible);

private:
                            MarbleRenderPlugin (const Marble::MarbleRenderPlugin&);
};   // MarbleRenderPlugin

};   // Marble


