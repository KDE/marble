// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Murad Tagirov <tmurad@gmail.com>
// SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "GeoDataObject.h"

#include <QDataStream>
#include <QFileInfo>
#include <QUrl>
#include <QtGlobal>

#include "GeoDataDocument.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataObjectPrivate
{
public:
    GeoDataObjectPrivate()
        : m_id()
        , m_targetId()
        , m_parent(nullptr)
    {
    }

    QString m_id;
    QString m_targetId;
    GeoDataObject *m_parent;
};

GeoDataObject::GeoDataObject()
    : GeoNode()
    , Serializable()
    , d(new GeoDataObjectPrivate())
{
}

GeoDataObject::GeoDataObject(GeoDataObject const &other)
    : GeoNode()
    , Serializable(other)
    , d(new GeoDataObjectPrivate(*other.d))
{
}

GeoDataObject &GeoDataObject::operator=(const GeoDataObject &rhs)
{
    *d = *rhs.d;
    return *this;
}

GeoDataObject::~GeoDataObject()
{
    delete d;
}

const GeoDataObject *GeoDataObject::parent() const
{
    return d->m_parent;
}

GeoDataObject *GeoDataObject::parent()
{
    return d->m_parent;
}

void GeoDataObject::setParent(GeoDataObject *parent)
{
    d->m_parent = parent;
}

QString GeoDataObject::id() const
{
    return d->m_id;
}

void GeoDataObject::setId(const QString &value)
{
    d->m_id = value;
}

QString GeoDataObject::targetId() const
{
    return d->m_targetId;
}

void GeoDataObject::setTargetId(const QString &value)
{
    d->m_targetId = value;
}

QString GeoDataObject::resolvePath(const QString &relativePath) const
{
    QUrl const url(relativePath);
    QFileInfo const fileInfo(url.path());
    if (url.isRelative() && fileInfo.isRelative()) {
        auto const *document = dynamic_cast<GeoDataDocument const *>(this);
        if (document) {
            QString const baseUri = document->baseUri();
            QFileInfo const documentRoot = QFileInfo(baseUri.isEmpty() ? document->fileName() : baseUri);
            QFileInfo const absoluteImage(documentRoot.absolutePath() + QLatin1Char('/') + url.path());
            return absoluteImage.absoluteFilePath();
        } else if (d->m_parent) {
            return d->m_parent->resolvePath(relativePath);
        }
    }

    return relativePath;
}

void GeoDataObject::pack(QDataStream &stream) const
{
    stream << d->m_id;
    stream << d->m_targetId;
}

void GeoDataObject::unpack(QDataStream &stream)
{
    stream >> d->m_id;
    stream >> d->m_targetId;
}

bool GeoDataObject::equals(const GeoDataObject &other) const
{
    return d->m_id == other.d->m_id && d->m_targetId == other.d->m_targetId;
}

}
