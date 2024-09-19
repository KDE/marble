/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GPX_ELEMENTDICTIONARY_H
#define MARBLE_GPX_ELEMENTDICTIONARY_H

namespace Marble
{

// Lists all known GPX 1.1 tags (https://www.topografix.com/GPX/1/1/)
namespace gpx
{
extern const char gpxTag_nameSpace10[];
extern const char gpxTag_nameSpace11[];

extern const char gpxTag_ele[];
extern const char gpxTag_extensions[];
extern const char gpxTag_gpx[];
extern const char gpxTag_lat[];
extern const char gpxTag_lon[];
extern const char gpxTag_name[];
extern const char gpxTag_time[];
extern const char gpxTag_desc[];
extern const char gpxTag_type[];
extern const char gpxTag_trk[];
extern const char gpxTag_trkpt[];
extern const char gpxTag_trkseg[];
extern const char gpxTag_wpt[];
extern const char gpxTag_rte[];
extern const char gpxTag_rtept[];
extern const char gpxTag_cmt[];
extern const char gpxTag_link[];
extern const char gpxTag_url[];
extern const char gpxTag_urlname[];
// TODO: add all remaining tags!

extern const char gpxTag_nameSpaceGarminTrackPointExt1[];
extern const char gpxTag_TrackPointExtension[];
extern const char gpxTag_hr[];
}

// Helper macros
#define GPX_DEFINE_TAG_HANDLER_10(Name) GEODATA_DEFINE_TAG_HANDLER(gpx, GPX, Name, gpxTag_nameSpace10)
#define GPX_DEFINE_TAG_HANDLER_11(Name) GEODATA_DEFINE_TAG_HANDLER(gpx, GPX, Name, gpxTag_nameSpace11)
#define GPX_DEFINE_TAG_HANDLER_GARMIN_TRACKPOINTEXT1(Name) GEODATA_DEFINE_TAG_HANDLER(gpx, GPX, Name, gpxTag_nameSpaceGarminTrackPointExt1)

#define GPX_DEFINE_TAG_HANDLER(Name)                                                                                                                           \
    GPX_DEFINE_TAG_HANDLER_10(Name)                                                                                                                            \
    GPX_DEFINE_TAG_HANDLER_11(Name)

}

#endif
