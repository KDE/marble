// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//

#include "NullTinyWebBrowser.h"
#include <QByteArray>

namespace Marble
{

TinyWebBrowser::TinyWebBrowser(QWidget *parent)
    : QWidget(parent)
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

#include "moc_NullTinyWebBrowser.cpp"
