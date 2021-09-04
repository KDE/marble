// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <gabor.peterffy@gmail.com>
//

#ifndef NULLMARBLEWEBVIEW_H
#define NULLMARBLEWEBVIEW_H

#include <QWidget>
#include <QUrl>

#include "marble_export.h"


class MARBLE_EXPORT MarbleWebView : public QWidget
{
    Q_OBJECT
public:
    explicit MarbleWebView(QWidget *parent = 0);
    void setUrl(const QUrl & url);
};

#endif // NULLMARBLEWEBVIEW_H
 
