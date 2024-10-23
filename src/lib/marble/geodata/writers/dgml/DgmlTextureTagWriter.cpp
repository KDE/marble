// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlTextureTagWriter.h"

#include "DgmlElementDictionary.h"
#include "DownloadPolicy.h"
#include "GeoSceneTileDataset.h"
#include "GeoSceneTypes.h"
#include "GeoWriter.h"
#include "ServerLayout.h"

#include <QUrl>

namespace Marble
{

static GeoTagWriterRegistrar s_writerTexture(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoSceneTypes::GeoSceneTileDatasetType),
                                                                         QString::fromLatin1(dgml::dgmlTag_nameSpace20)),
                                             new DgmlTextureTagWriter());

bool DgmlTextureTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto texture = static_cast<const GeoSceneTileDataset *>(node);
    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Texture));
    writer.writeAttribute(QStringLiteral("name"), texture->name());
    writer.writeAttribute("expire", QString::number(texture->expire()));

    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_SourceDir));
    writer.writeAttribute(QStringLiteral("format"), texture->fileFormat());
    if (texture->expire()) {
        writer.writeAttribute("expire", QString::number(texture->expire()));
    }
    writer.writeCharacters(texture->sourceDir());
    writer.writeEndElement();
    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_TileSize));
    writer.writeAttribute("width", QString::number(texture->tileSize().width()));
    writer.writeAttribute("height", QString::number(texture->tileSize().height()));
    writer.writeEndElement();

    writer.writeOptionalElement(QString::fromLatin1(dgml::dgmlTag_InstallMap), texture->installMap());

    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_StorageLayout));
    if (texture->hasMaximumTileLevel()) {
        writer.writeAttribute("maximumTileLevel", QString::number(texture->maximumTileLevel()));
        writer.writeAttribute("levelZeroColumns", QString::number(texture->levelZeroColumns()));
        writer.writeAttribute("levelZeroRows", QString::number(texture->levelZeroRows()));
        writer.writeAttribute("mode", texture->serverLayout()->name());
    }
    writer.writeEndElement();

    if (!texture->downloadUrls().isEmpty()) {
        for (int i = 0; i < texture->downloadUrls().size(); ++i) {
            QString protocol = texture->downloadUrls().at(i).toString().left(texture->downloadUrls().at(i).toString().indexOf(QLatin1Char(':')));
            QString host = QString(texture->downloadUrls().at(i).host());
            int port = texture->downloadUrls().at(i).port();
            QString path = QString(texture->downloadUrls().at(i).path());
            QString query = texture->downloadUrls().at(i).query(QUrl::FullyEncoded);

            writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_DownloadUrl));
            writer.writeAttribute("protocol", protocol);
            writer.writeAttribute("host", host);
            if (port != -1) {
                writer.writeAttribute("port", QString::number(port));
            }
            writer.writeAttribute("path", path);
            writer.writeAttribute("query", query);
            writer.writeEndElement();
        }
    }

    for (const DownloadPolicy *policy : texture->downloadPolicies()) {
        writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_DownloadPolicy));

        if (policy->key().usage() == DownloadBrowse) {
            writer.writeAttribute("usage", "Browse");
            writer.writeAttribute("maximumConnections", QString::number(policy->maximumConnections()));
        }

        else if (policy->key().usage() == DownloadBulk) {
            writer.writeAttribute("usage", "Bulk");
            writer.writeAttribute("maximumConnections", QString::number(policy->maximumConnections()));
        }

        writer.writeEndElement();
    }

    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Projection));
    const GeoSceneAbstractTileProjection::Type tileProjectionType = texture->tileProjectionType();
    if (tileProjectionType == GeoSceneAbstractTileProjection::Mercator) {
        writer.writeAttribute(QStringLiteral("name"), "Mercator");
    } else if (tileProjectionType == GeoSceneAbstractTileProjection::Equirectangular) {
        writer.writeAttribute(QStringLiteral("name"), "Equirectangular");
    }
    writer.writeEndElement();

    writer.writeStartElement(QString::fromUtf8(dgml::dgmlTag_Blending));
    if (!texture->blending().isEmpty()) {
        writer.writeAttribute(QStringLiteral("name"), texture->blending());
    }
    writer.writeEndElement();

    writer.writeEndElement();
    return true;
}

}
