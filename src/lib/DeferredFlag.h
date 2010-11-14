//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// DeferredFlag paints a flag on a pixmap, and sends a signal when its
// done so that you can do it asynchronously.
//

#ifndef MARBLE_DEFERREDFLAG_H
#define MARBLE_DEFERREDFLAG_H


#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtGui/QPixmap>

namespace Marble
{

class DeferredFlag : public QObject
{

    Q_OBJECT

 public:
    explicit DeferredFlag( QObject *parent = 0 );

    QPixmap& flag(){ return m_pixmap; }
    void setFlag( const QString& filename, const QSize& size );

 public Q_SLOTS:
    void slotDrawFlag();

 Q_SIGNALS:
    void flagDone();

 private:
    QString  m_filename;
    QSize    m_size;
    QPixmap  m_pixmap;
};

}

#endif
