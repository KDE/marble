// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

#include "MarbleMaps.h"
#include <FileManager.h>

#include <QGuiApplication>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>
#include <qandroidfunctions.h>
#endif

namespace Marble
{

MarbleMapsApplication::MarbleMapsApplication(QObject *parent)
    : QObject(parent)
    , m_suspended(false)
    , m_keepScreenOn(false)
{
    auto application = qobject_cast<QGuiApplication *>(QGuiApplication::instance());
    if (application) {
        connect(application, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this, SLOT(handleApplicationStateChange(Qt::ApplicationState)));
    }

#ifdef Q_OS_ANDROID
    QAndroidJniObject const activity = QtAndroid::androidActivity();
    if (activity.isValid()) {
        // Control music volume
        int const STREAM_MUSIC = 3;
        activity.callMethod<void>("setVolumeControlStream", "(I)V", STREAM_MUSIC);

        // If a file is passed, open it. Possible file types are registered in package/AndroidManifest.xml
        QAndroidJniObject const intent = activity.callObjectMethod("getIntent", "()Landroid/content/Intent;");
        if (intent.isValid()) {
            QAndroidJniObject const data = intent.callObjectMethod("getData", "()Landroid/net/Uri;");
            if (data.isValid()) {
                QAndroidJniObject const path = data.callObjectMethod("getPath", "()Ljava/lang/String;");
                if (path.isValid()) {
                    model()->addGeoDataFile(path.toString());
                    connect(model()->fileManager(), SIGNAL(centeredDocument(GeoDataLatLonBox)), this, SLOT(centerOn(GeoDataLatLonBox)));
                }
            }
        }
    }
#endif
}

bool MarbleMapsApplication::isSuspended() const
{
    return m_suspended;
}

bool MarbleMapsApplication::keepScreenOn() const
{
    return m_keepScreenOn;
}

void MarbleMapsApplication::setKeepScreenOn(bool screenOn)
{
    if (m_keepScreenOn == screenOn) {
        return;
    }
    m_keepScreenOn = screenOn;
    char const *const action = m_keepScreenOn ? "addFlags" : "clearFlags";
#ifdef Q_OS_ANDROID
    QtAndroid::runOnAndroidThread([action]() {
        QAndroidJniObject activity = QtAndroid::androidActivity();
        if (activity.isValid()) {
            QAndroidJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
            if (window.isValid()) {
                const int FLAG_KEEP_SCREEN_ON = 128;
                window.callMethod<void>(action, "(I)V", FLAG_KEEP_SCREEN_ON);
            }
        }
    });
#else
    Q_UNUSED(action);
#endif
    Q_EMIT keepScreenOnChanged(screenOn);
}

void MarbleMapsApplication::handleApplicationStateChange(Qt::ApplicationState state)
{
    if (state == Qt::ApplicationSuspended) {
        m_suspended = true;
        Q_EMIT isSuspendedChanged(m_suspended);
    } else if (state == Qt::ApplicationActive) {
        m_suspended = false;
        Q_EMIT isSuspendedChanged(m_suspended);
    }
}

}

#include "moc_MarbleMaps.cpp"
