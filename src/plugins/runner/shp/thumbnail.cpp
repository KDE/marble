// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2025 Nicolas Fella <nicolas.fella@gmx.de>

#include "thumbnailer.h"

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(Marble::GeoDataThumbnailer, "marble_thumbnail_shp.json")

#include "thumbnail.moc"
