//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

#include "MarbleMaps.h"
#include <FileManager.h>

#include <MarbleModel.h>
#include <QGuiApplication>

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#endif

namespace Marble {

MarbleMaps::MarbleMaps(QQuickItem *parent) :
    MarbleQuickItem(parent),
    m_suspended(false)
{
    QGuiApplication* application = qobject_cast<QGuiApplication*>(QGuiApplication::instance());
    if (application) {
        connect(application, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
                this, SLOT(handleApplicationStateChange(Qt::ApplicationState)));
    }

#ifdef Q_OS_ANDROID
    // If a file is passed, open it. Possible file types are registered in package/AndroidManifest.xml
    QAndroidJniObject const activity = QtAndroid::androidActivity();
    if (activity.isValid()) {
        QAndroidJniObject const intent = activity.callObjectMethod("getIntent", "()Landroid/content/Intent;");
        if (intent.isValid()) {
            QAndroidJniObject const data = intent.callObjectMethod("getData", "()Landroid/net/Uri;");
            if (data.isValid()) {
                QAndroidJniObject const path = data.callObjectMethod("getPath", "()Ljava/lang/String;");
                if (path.isValid()) {
                    model()->addGeoDataFile(path.toString());
                    connect( model()->fileManager(), SIGNAL(centeredDocument(GeoDataLatLonBox)), this, SLOT(centerOn(GeoDataLatLonBox)) );
                }
            }
        }
    }
#endif
}

bool MarbleMaps::isSuspended() const
{
    return m_suspended;
}

void MarbleMaps::handleApplicationStateChange(Qt::ApplicationState state)
{
    if (state == Qt::ApplicationSuspended) {
        m_suspended = true;
        emit isSuspendedChanged(m_suspended);
    } else if (state == Qt::ApplicationActive) {
        m_suspended = false;
        emit isSuspendedChanged(m_suspended);
    }
}

}

#include "moc_MarbleMaps.cpp"
