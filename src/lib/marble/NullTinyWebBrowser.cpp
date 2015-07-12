//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015 Gábor Péterffy <peterffy95@gmail.com>
//

#include "NullTinyWebBrowser.h"
#include <QByteArray>

namespace Marble {

TinyWebBrowser::TinyWebBrowser(QWidget *parent):
    QWidget(parent)
{
}

QByteArray TinyWebBrowser::userAgent(const QString &platform, const QString &plugin)
{
    return QByteArray();
}

void TinyWebBrowser::setWikipediaPath(const QString &relativeUrl)
{
}

void TinyWebBrowser::print()
{
}

}
