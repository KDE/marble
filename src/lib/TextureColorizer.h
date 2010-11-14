//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// The TextureColorizer maps the Elevationvalues to Legend Colors.
//

#ifndef MARBLE_TEXTURECOLORIZER_H
#define MARBLE_TEXTURECOLORIZER_H

#include <QtCore/QString>

namespace Marble
{

class ViewParams;

class TextureColorizer
{
 public:
    TextureColorizer();
    virtual ~TextureColorizer(){}

    void colorize(ViewParams *viewParams);

    void setSeaFileLandFile( const QString& seafile,
                             const QString& landfile );
 private:
    QString m_seafile;
    QString m_landfile;
    uint texturepalette[16][512];
};

}

#endif
