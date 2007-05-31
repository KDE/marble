//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef TILESCISSOR_H
#define TILESCISSOR_H


#include <QtCore/QObject>
#include <QtCore/QString>


class TileScissor : public QObject
{
    Q_OBJECT

 public: 
    TileScissor( const QString& prefix, const QString& installmap, 
                 const QString& dem );

 signals:
    void  progress( int value );

 public slots:
    void  createTiles();

 private:
    QString  m_prefix;
    QString  m_installmap;
    QString  m_dem;
};


#endif
