// Copyright 2014 Friedrich W. H. Kossebau <kossebau@kde.org>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program. If not, see <http://www.gnu.org/licenses/>.


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
    virtual ~GeoDataThumbnailer();

public: // ThumbCreator API
    virtual bool create(const QString &path, int width, int height, QImage &image);
    virtual Flags flags() const;

private Q_SLOTS:
    void onGeoDataObjectAdded(GeoDataObject *object);

private:
    MarbleMap m_marbleMap;

    QString m_currentFilename;
    bool m_loadingCompleted :1;
    QEventLoop m_eventLoop;
    QTimer m_outtimer;
};

}

#endif
