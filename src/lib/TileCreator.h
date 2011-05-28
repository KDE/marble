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
#include <QtCore/QSize>
#include <QtGui/QImage>

#include "marble_export.h"

namespace Marble
{

class TileCreatorPrivate;

class MARBLE_EXPORT TileCreatorSource
{
public:
    virtual ~TileCreatorSource() {}
    virtual QSize fullImageSize() const = 0;
    virtual QImage tile( int n, int m, int maxTileLevel ) = 0;
    virtual QString sourcePath() const = 0;
};

class MARBLE_EXPORT TileCreator : public QThread
{
    Q_OBJECT

 public:
    /*
     * Constructor for standard Image source
     */
    TileCreator( const QString& sourceDir, const QString& installMap, 
                 const QString& dem,       const QString& targetDir=QString() );

    /*
     * Constructor for own, custom source class
     */
    TileCreator( TileCreatorSource *source, const QString& dem, const QString& targetDir );

    virtual ~TileCreator();

    void cancelTileCreation();

    void setTileFormat( const QString &format );
    void setTileQuality( int quality );
    void setResume( bool resume );
    void setVerifyExactResult( bool verify );
    QString tileFormat() const;
    int tileQuality() const;
    bool resume() const;
    bool verifyExactResult() const;

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
