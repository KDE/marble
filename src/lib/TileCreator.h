//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
//

#ifndef MARBLE_TILECREATOR_H
#define MARBLE_TILECREATOR_H


#include <QtCore/QString>
#include <QtCore/QThread>

#include "marble_export.h"

namespace Marble
{

class TileCreatorPrivate;


class MARBLE_EXPORT TileCreator : public QThread
{
    Q_OBJECT

 public: 
    TileCreator( const QString& sourceDir, const QString& installMap, 
                 const QString& dem,       const QString& targetDir=QString() );
    virtual ~TileCreator();

    void cancelTileCreation();

 protected:
    virtual void run();

 Q_SIGNALS:
    void  progress( int value );


 private:
    Q_DISABLE_COPY( TileCreator )
    TileCreatorPrivate  * const d;
};

}

#endif
