//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// KAtlasFlag paints a flag on a pixmap
//


#ifndef KATLASFLAG_H
#define KATLASFLAG_H


#include <QtCore/QObject>
#include <QtGui/QPixmap>


class KAtlasFlag : public QObject
{

    Q_OBJECT

 public:
    KAtlasFlag( QObject *parent = 0 );

    QPixmap& flag(){ return m_pixmap; }
    void setFlag( const QString& filename, const QSize& size );

 public slots:
    void slotDrawFlag();

 signals:
    void flagDone();

 private:
    QString  m_filename;
    QSize    m_size;
    QPixmap  m_pixmap;
};


#endif // KATLASFLAG_H
