// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Constantin Mihalache <mihalache.c94@gmail.com>
//

// self
#include "OpenLocationCodeSearchRunner.h"

#include "GeoDataLineStyle.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataPolygon.h"
#include "GeoDataStyle.h"

// Qt
#include <QColor>
#include <QList>
#include <QRegularExpression>

namespace Marble
{

OpenLocationCodeSearchRunner::OpenLocationCodeSearchRunner(QObject *parent)
    : SearchRunner(parent)
{
    // initialize the charIndex map
    QString const acceptedChars = QStringLiteral("23456789CFGHJMPQRVWX");
    for (int index = 0; index < acceptedChars.size(); index++) {
        charIndex[acceptedChars[index]] = index;
    }
}

void OpenLocationCodeSearchRunner::search(const QString &searchTerm, const GeoDataLatLonBox &preferred)
{
    Q_UNUSED(preferred);

    QList<GeoDataPlacemark *> result;

    if (isValidOLC(searchTerm.toUpper())) {
        GeoDataLatLonBox boundingBox = decodeOLC(searchTerm.toUpper());
        if (!boundingBox.isEmpty()) {
            auto placemark = new GeoDataPlacemark(searchTerm);

            auto geometry = new GeoDataPolygon(polygonFromLatLonBox(boundingBox));
            placemark->setGeometry(geometry);

            GeoDataStyle::Ptr style = GeoDataStyle::Ptr(new GeoDataStyle());
            GeoDataLineStyle lineStyle;
            GeoDataPolyStyle polyStyle;
            lineStyle.setColor(QColor(Qt::GlobalColor::red));
            lineStyle.setWidth(2);
            polyStyle.setFill(false);
            style->setLineStyle(lineStyle);
            style->setPolyStyle(polyStyle);
            placemark->setStyle(style);

            result.append(placemark);
        }
    }

    Q_EMIT searchFinished(result);
}

GeoDataPolygon OpenLocationCodeSearchRunner::polygonFromLatLonBox(const GeoDataLatLonBox &boundingBox) const
{
    if (boundingBox.isEmpty()) {
        return GeoDataPolygon();
    }

    GeoDataPolygon poly;
    GeoDataLinearRing outerBoundry;
    // north-west corner
    outerBoundry.append(GeoDataCoordinates(boundingBox.west(), boundingBox.north(), GeoDataCoordinates::Unit::Degree));
    // north-east corner
    outerBoundry.append(GeoDataCoordinates(boundingBox.east(), boundingBox.north(), GeoDataCoordinates::Unit::Degree));
    // south-east corner
    outerBoundry.append(GeoDataCoordinates(boundingBox.east(), boundingBox.south(), GeoDataCoordinates::Unit::Degree));
    // south-west corner
    outerBoundry.append(GeoDataCoordinates(boundingBox.west(), boundingBox.south(), GeoDataCoordinates::Unit::Degree));

    poly.setOuterBoundary(outerBoundry);

    return poly;
}

GeoDataLatLonBox OpenLocationCodeSearchRunner::decodeOLC(const QString &olc) const
{
    if (!isValidOLC(olc)) {
        return {};
    }

    // remove padding
    QString decoded = olc;
    decoded = decoded.remove(QRegularExpression(QStringLiteral("[0+]")));
    qreal southLatitude = 0;
    qreal westLongitude = 0;

    int digit = 0;
    qreal latitudeResolution = 400;
    qreal longitudeResolution = 400;

    while (digit < decoded.size()) {
        if (digit < 10) {
            latitudeResolution /= 20;
            longitudeResolution /= 20;
            southLatitude += latitudeResolution * charIndex[decoded[digit]];
            westLongitude += longitudeResolution * charIndex[decoded[digit + 1]];
            digit += 2;
        } else {
            latitudeResolution /= 5;
            longitudeResolution /= 4;
            southLatitude += latitudeResolution * (charIndex[decoded[digit]] / 4);
            westLongitude += longitudeResolution * (charIndex[decoded[digit]] % 4);
            digit += 1;
        }
    }
    return {southLatitude - 90 + latitudeResolution,
            southLatitude - 90,
            westLongitude - 180 + longitudeResolution,
            westLongitude - 180,
            GeoDataCoordinates::Unit::Degree};
}

bool OpenLocationCodeSearchRunner::isValidOLC(const QString &olc) const
{
    // It must have only one SEPARATOR located at an even index in
    // the string.
    QChar const separator(QLatin1Char('+'));
    int separatorPos = olc.indexOf(separator);
    if (separatorPos == -1 || separatorPos != olc.lastIndexOf(separator) || separatorPos % 2 != 0) {
        return false;
    }
    int const separatorPosition = 8;
    // It must be a full open location code.
    if (separatorPos != separatorPosition) {
        return false;
    }

    // Test the first two characters as only some characters of the
    // ACCEPTED_CHARS are allowed.
    //
    // First latitude character can only take one of the first 9 values.
    int index0 = charIndex.value(olc[0], -1);
    if (index0 == -1 || index0 > 8) {
        return false;
    }
    // First longitude character can only take one of the first 18 values.
    int index1 = charIndex.value(olc[1], -1);
    if (index1 == -1 || index1 > 17) {
        return false;
    }

    // Test the characters before the SEPARATOR.
    QChar const suffixPadding(QLatin1Char('0'));
    bool paddingBegun = false;
    for (int index = 0; index < separatorPos; index++) {
        if (paddingBegun) {
            // Once padding has begun, there should be only padding.
            if (olc[index] != suffixPadding) {
                return false;
            }
            continue;
        }
        if (charIndex.contains(olc[index])) {
            continue;
        }
        if (olc[index] == suffixPadding) {
            paddingBegun = true;
            // Padding can start only at an even index.
            if (index % 2 != 0) {
                return false;
            }
            continue;
        }
        return false;
    }

    // Test the characters after the SEPARATOR.
    if (olc.size() > separatorPos + 1) {
        if (paddingBegun) {
            return false;
        }
        // Only one character after the SEPARATOR is not allowed.
        if (olc.size() == separatorPos + 2) {
            return false;
        }
        for (int index = separatorPos + 1; index < olc.size(); index++) {
            if (!charIndex.contains(olc[index])) {
                return false;
            }
        }
    }

    return true;
}

}

#include "moc_OpenLocationCodeSearchRunner.cpp"
