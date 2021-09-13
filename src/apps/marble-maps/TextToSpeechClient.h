// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//

#ifndef TEXTTOSPEECHCLIENT_H
#define TEXTTOSPEECHCLIENT_H

#include <QObject>

class TextToSpeechClient : public QObject
{
    Q_OBJECT

public:
    explicit TextToSpeechClient(QObject * parent = nullptr);
    ~TextToSpeechClient() override;

public Q_SLOTS:
    Q_INVOKABLE void readText(const QString & text);
    Q_INVOKABLE void setLocale(const QString & locale);
};

#endif
