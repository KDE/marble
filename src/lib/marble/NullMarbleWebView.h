//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy   <gabor.peterffy@gmail.com>
//

#ifndef NULLMARBLEWEBVIEW_H
#define NULLMARBLEWEBVIEW_H

#include <QWidget>
#include <QUrl>

#include "marble_export.h"

class QUrl;

class MARBLE_EXPORT MarbleWebView : public QWidget
{
    Q_OBJECT
public:
    explicit MarbleWebView(QWidget *parent = 0);
    void setUrl(const QUrl & url);
};

#endif // NULLMARBLEWEBVIEW_H
 
