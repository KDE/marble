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

class TileCreator : QThread
{
%TypeHeaderCode
#include <TileCreator.h>
%End

public:
                            TileCreator (const QString& sourceDir, const QString& installMap, const QString& dem, const QString& targetDir = QString());
    void                    cancelTileCreation ();

protected:
    virtual void            run ();

signals:
    void                    progress (int value);

public:
    virtual ~TileCreator ();
                            TileCreator (Marble::TileCreatorSource* source, const QString& dem, const QString& targetDir);
    void                    setTileFormat (const QString& format);
    void                    setTileQuality (int quality);
    void                    setResume (bool resume);
    void                    setVerifyExactResult (bool verify);
    QString                 tileFormat () const;
    int                     tileQuality () const;
    bool                    resume () const;
    bool                    verifyExactResult () const;
};
// TileCreator

class TileCreatorSource
{
%TypeHeaderCode
#include <TileCreator.h>
%End

public:
    virtual ~TileCreatorSource ();
    virtual QSize           fullImageSize () const=0;
    virtual QImage          tile (int n, int m, int tileLevel)=0;
};
};
// Marble


