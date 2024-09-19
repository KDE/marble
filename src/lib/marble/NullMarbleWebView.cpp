// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <gabor.peterffy@gmail.com>
//

#include "NullMarbleWebView.h"

MarbleWebView::MarbleWebView(QWidget *parent)
    : QWidget(parent)
{
}

void MarbleWebView::setUrl(const QUrl &url)
{
}

#include "moc_NullMarbleWebView.cpp"
