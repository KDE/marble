// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//

#include "TextToSpeechClient.h"

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

TextToSpeechClient::TextToSpeechClient(QObject *parent)
    : QObject(parent)
{
    // TODOShip translations and only fall back to english if no translations for the system locale are installed
    setLocale("en");
}

TextToSpeechClient::~TextToSpeechClient() = default;

void TextToSpeechClient::readText(const QString &text)
{
#ifdef Q_OS_ANDROID
    QAndroidJniObject jniText = QAndroidJniObject::fromString(text);
    QAndroidJniObject::callStaticMethod<void>("org/kde/marble/maps/TextToSpeechClient", "read", "(Ljava/lang/String;)V", jniText.object<jstring>());
#else
    Q_UNUSED(text)
#endif
}

void TextToSpeechClient::setLocale(const QString &locale)
{
#ifdef Q_OS_ANDROID
    QAndroidJniObject jniText = QAndroidJniObject::fromString(locale);
    QAndroidJniObject::callStaticMethod<void>("org/kde/marble/maps/TextToSpeechClient",
                                              "initSpeakerStatic",
                                              "(Ljava/lang/String;)V",
                                              jniText.object<jstring>());
#else
    Q_UNUSED(locale)
#endif
}

#include "moc_TextToSpeechClient.cpp"
