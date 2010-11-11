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

class MarbleDirs
{
%TypeHeaderCode
#include <MarbleDirs.h>
%End

public:
                            MarbleDirs ();
    static QString          path (const QString& relativePath);
    static QString          pluginPath (const QString& relativePath);
    static QStringList      entryList (const QString& relativePath, QDir::Filters filters = QDir::NoFilter);
    static QStringList      pluginEntryList (const QString& relativePath, QDir::Filters filters = QDir::NoFilter);
    static QString          systemPath ();
    static QString          pluginSystemPath ();
    static QString          localPath ();
    static QString          pluginLocalPath ();
    static QString          marbleDataPath ();
    static QString          marblePluginPath ();
    static void             setMarbleDataPath (const QString& adaptedPath);
    static void             setMarblePluginPath (const QString& adaptedPath);
    static void             debug ();
private:
//force
                            MarbleDirs (const Marble::MarbleDirs& other);
//end
public:
    static QStringList      oldLocalPaths ();
};
// MarbleDirs

};
// Marble


