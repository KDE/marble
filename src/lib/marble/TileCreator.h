// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2004-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007-2008 Inge Wallin <ingwa@kde.org>
//

#ifndef MARBLE_TILECREATOR_H
#define MARBLE_TILECREATOR_H

#include <QString>
#include <QThread>

#include "marble_export.h"

class QSize;
class QImage;

namespace Marble
{

class TileCreatorPrivate;

/**
 * Base Class for custom tile source
 *
 * Implement this class to have more control over the tile creating process. This
 * is needed when creating with a high tileLevel where the whole source image can't
 * be loaded at once.
 **/
class MARBLE_EXPORT TileCreatorSource
{
public:
    virtual ~TileCreatorSource() {}

    /**
     * Must return the full size of the source image
     */
    virtual QSize fullImageSize() const = 0;

    /**
     * Must return one specific tile
     *
     * tileLevel can be used to calculate the number of tiles in a row or column
     */
    virtual QImage tile( int n, int m, int tileLevel ) = 0;
};

class MARBLE_EXPORT TileCreator : public QThread
{
    Q_OBJECT

 public:
    /**
     * Constructor for standard Image source
     */
    TileCreator( const QString& sourceDir, const QString& installMap, 
                 const QString& dem,       const QString& targetDir=QString() );

    /**
     * Constructor for own, custom source class
     *
     * Ownership of source is taken by TileCreator
     */
    TileCreator( TileCreatorSource *source, const QString& dem, const QString& targetDir );

    ~TileCreator() override;

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
    void run() override;

 Q_SIGNALS:
    void  progress( int value );


 private:
    Q_DISABLE_COPY( TileCreator )
    TileCreatorPrivate  * const d;
};

}

#endif
