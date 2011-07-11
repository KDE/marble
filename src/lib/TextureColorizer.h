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

#include "VectorComposer.h"

#include <QtCore/QString>

namespace Marble
{

class ViewParams;

class TextureColorizer : public QObject
{
    Q_OBJECT

 public:
    TextureColorizer( const QString &seafile,
                      const QString &landfile,
                      QObject *parent = 0 );

    virtual ~TextureColorizer(){}

    void setShowRelief( bool show );

    void colorize(ViewParams *viewParams);

 Q_SIGNALS:
    void datasetLoaded();

 private:
    VectorComposer m_veccomposer;
    QString m_seafile;
    QString m_landfile;
    uint texturepalette[16][512];
    bool m_showRelief;
};

}

#endif
