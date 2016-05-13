//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
//

#ifndef TEXTTOSPEECHCLIENT_H
#define TEXTTOSPEECHCLIENT_H

#include <QObject>

class TextToSpeechClient : public QObject
{
    Q_OBJECT

public:
    explicit TextToSpeechClient(QObject * parent = 0);
    ~TextToSpeechClient();

public Q_SLOTS:
    Q_INVOKABLE void readText(const QString & text);
    Q_INVOKABLE void setLocale(const QString & locale);
};

#endif
