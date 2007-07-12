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

#include "KMLFolder.h"
#include "PlaceMarkContainer.h"

class QIODevice;

class KMLDocument : public KMLFolder
{
 public:
    KMLDocument();

    void load( QIODevice& source );

    /*
     * Will move this method to KMLDocumentModel in a next step
     */
    PlaceMarkContainer& activePlaceMarkContainer();

 private:
     PlaceMarkContainer m_activePlaceMarkContainer;
};

#endif // KMLDOCUMENT_H
