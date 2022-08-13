// SPDX-FileCopyrightText: 2014 Friedrich W. H. Kossebau <kossebau@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later


#ifndef _MARBLEGEODATATHUMBNAILER_H_
#define _MARBLEGEODATATHUMBNAILER_H_

// Marble
#include "MarbleMap.h"

// KF
#include <KIOWidgets/kio/thumbcreator.h>

// Qt
#include <QEventLoop>
#include <QTimer>

namespace Marble
{

class GeoDataObject;


class GeoDataThumbnailer : public QObject, public ThumbCreator
{
    Q_OBJECT

public:
    GeoDataThumbnailer();
    ~GeoDataThumbnailer() override;

public: // ThumbCreator API
    bool create(const QString &path, int width, int height, QImage &image) override;

private:
    void onGeoDataObjectAdded(GeoDataObject *object);

    MarbleMap m_marbleMap;

    QString m_currentFilename;
    bool m_loadingCompleted :1;
    bool m_hadErrors :1;
    QEventLoop m_eventLoop;
    QTimer m_outtimer;
};

}

#endif
