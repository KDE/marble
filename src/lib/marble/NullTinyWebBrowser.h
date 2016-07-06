//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015 Gábor Péterffy <peterffy95@gmail.com>
//

#ifndef NULLTINYWEBBROWSER_H
#define NULLTINYWEBBROWSER_H

// Qt
#include <QWidget>

// Marble
#include "marble_export.h"

class QString;
class QByteArray;

namespace Marble
{

class MARBLE_EXPORT TinyWebBrowser : public QWidget
{
    Q_OBJECT

 public:
    explicit TinyWebBrowser( QWidget* parent = 0 );

    static QByteArray userAgent(const QString &platform, const QString &plugin);

 public Q_SLOTS:
    void setWikipediaPath( const QString& relativeUrl );
    void print();

 Q_SIGNALS:
    void statusMessage( const QString& );
};

}

#endif
