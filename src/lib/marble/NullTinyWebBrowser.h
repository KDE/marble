// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
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
