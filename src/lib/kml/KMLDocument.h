//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLDOCUMENT_H
#define KMLDOCUMENT_H

#include <QtCore/QList>
#include "KMLFolder.h"

class QIODevice;
class KMLStyle;

class KMLDocument : public KMLFolder
{
  public:
    KMLDocument();
    ~KMLDocument();

    void load( QIODevice& source );

    void addStyle( KMLStyle* style );
    const KMLStyle& getStyle( QString styleId ) const;

  private:
    QHash < QString, KMLStyle* > m_styleHash;
};

#endif // KMLDOCUMENT_H
