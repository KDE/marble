// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Adrian Draghici <draghici.adrian.b@gmail.com>
//

#include "KmlPlaylistTagWriter.h"

#include "GeoDataAnimatedUpdate.h"
#include "GeoDataFlyTo.h"
#include "GeoDataPlaylist.h"
#include "GeoDataSoundCue.h"
#include "GeoDataTypes.h"
#include "GeoDataWait.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerPlaylist(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataPlaylistType),
                                                                          QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                              new KmlPlaylistTagWriter);

bool KmlPlaylistTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto playlist = static_cast<const GeoDataPlaylist *>(node);

    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_nameSpaceGx22), QString::fromUtf8(kml::kmlTag_Playlist));

    for (int i = 0; i < playlist->size(); i++) {
        writeTourPrimitive(playlist->primitive(i), writer);
    }

    writer.writeEndElement();

    return true;
}

void KmlPlaylistTagWriter::writeTourPrimitive(const GeoDataTourPrimitive *primitive, GeoWriter &writer)
{
    if (const auto tourControl = geodata_cast<GeoDataTourControl>(primitive)) {
        writeTourControl(*tourControl, writer);
    } else if (const auto wait = geodata_cast<GeoDataWait>(primitive)) {
        writeWait(*wait, writer);
    } else if (const auto flyTo = geodata_cast<GeoDataFlyTo>(primitive)) {
        writeElement(flyTo, writer);
    } else if (const auto soundCue = geodata_cast<GeoDataSoundCue>(primitive)) {
        writeSoundCue(*soundCue, writer);
    } else if (const auto animatedUpdate = geodata_cast<GeoDataAnimatedUpdate>(primitive)) {
        writeElement(animatedUpdate, writer);
    }
}

void KmlPlaylistTagWriter::writeTourControl(const GeoDataTourControl &tourControl, GeoWriter &writer)
{
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_nameSpaceGx22), QString::fromUtf8(kml::kmlTag_TourControl));
    KmlObjectTagWriter::writeIdentifiers(writer, &tourControl);

    writer.writeElement(QString::fromLatin1(kml::kmlTag_nameSpaceGx22), QString::fromLatin1(kml::kmlTag_playMode), playModeToString(tourControl.playMode()));

    writer.writeEndElement();
}

void KmlPlaylistTagWriter::writeWait(const GeoDataWait &wait, GeoWriter &writer)
{
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_nameSpaceGx22), QString::fromUtf8(kml::kmlTag_Wait));
    KmlObjectTagWriter::writeIdentifiers(writer, &wait);

    writer.writeElement(QString::fromLatin1(kml::kmlTag_nameSpaceGx22), QString::fromLatin1(kml::kmlTag_duration), QString::number(wait.duration()));

    writer.writeEndElement();
}

void KmlPlaylistTagWriter::writeSoundCue(const GeoDataSoundCue &cue, GeoWriter &writer)
{
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_nameSpaceGx22), QString::fromUtf8(kml::kmlTag_SoundCue));
    KmlObjectTagWriter::writeIdentifiers(writer, &cue);

    writer.writeElement(QString::fromLatin1(kml::kmlTag_href), cue.href());
    writer.writeElement(QString::fromLatin1(kml::kmlTag_nameSpaceGx22), QString::fromLatin1(kml::kmlTag_delayedStart), QString::number(cue.delayedStart()));

    writer.writeEndElement();
}

QString KmlPlaylistTagWriter::playModeToString(GeoDataTourControl::PlayMode playMode)
{
    switch (playMode) {
    case GeoDataTourControl::Play:
        return QStringLiteral("play");
    case GeoDataTourControl::Pause:
        return QStringLiteral("pause");
    default:
        return {};
    }
}

}
