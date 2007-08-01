//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//

#ifndef KMLFOLDER_H
#define KMLFOLDER_H

#include <QtCore/QVector>
#include "KMLContainer.h"

class KMLFolder : public KMLContainer
{
 public:
    KMLFolder();
    ~KMLFolder();

    void addFolder( KMLFolder* folder );

 protected:
    QVector <KMLFolder*> m_folderVector;
};

#endif // KMLFOLDER_H
