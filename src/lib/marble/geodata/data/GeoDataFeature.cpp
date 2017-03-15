//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataFeature.h"
#include "GeoDataFeature_p.h"

#include <QDataStream>
#include <QSize>

#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"

#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataNetworkLink.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataPlacemark.h"
#include "GeoDataScreenOverlay.h"
#include "GeoDataTour.h"
#include "GeoDataRegion.h"
#include "GeoDataCamera.h"

namespace Marble
{

const QSharedPointer<const GeoDataStyle> GeoDataFeaturePrivate::s_defaultStyle(new GeoDataStyle);

GeoDataFeature::GeoDataFeature()
    : d_ptr(new GeoDataFeaturePrivate())
{
}

GeoDataFeature::GeoDataFeature( const GeoDataFeature& other )
    : GeoDataObject(),
      d_ptr(new GeoDataFeaturePrivate(*other.d_ptr))
{
}

GeoDataFeature::GeoDataFeature( const QString& name )
    : d_ptr(new GeoDataFeaturePrivate())
{
    d_ptr->m_name = name;
}

GeoDataFeature::GeoDataFeature(GeoDataFeaturePrivate *dd)
    : GeoDataObject(),
      d_ptr(dd)
{
}

GeoDataFeature::GeoDataFeature(const GeoDataFeature& other, GeoDataFeaturePrivate *dd)
    : GeoDataObject(),
      d_ptr(dd)
{
    Q_UNUSED(other);
    // TODO: some classes pass "other" on and thus get duplicated id, also in operator=. Align behaviour
}

GeoDataFeature::~GeoDataFeature()
{
    delete d_ptr;
}

GeoDataFeature& GeoDataFeature::operator=( const GeoDataFeature& other )
{
    if (this != &other) {
        *d_ptr = *other.d_ptr;
    }

    return *this;
}

bool GeoDataFeature::operator==(const GeoDataFeature &other) const
{
    if (nodeType() != other.nodeType()) {
        return false;
    }

    if (nodeType() == GeoDataTypes::GeoDataDocumentType) {
        const GeoDataDocument &thisDoc = static_cast<const GeoDataDocument &>(*this);
        const GeoDataDocument &otherDoc = static_cast<const GeoDataDocument &>(other);

        return thisDoc == otherDoc;
    } else if (nodeType() == GeoDataTypes::GeoDataFolderType) {
        const GeoDataFolder &thisFolder = static_cast<const GeoDataFolder &>(*this);
        const GeoDataFolder &otherFolder = static_cast<const GeoDataFolder &>(other);

        return thisFolder == otherFolder;
    } else if (nodeType() == GeoDataTypes::GeoDataGroundOverlayType) {
        const GeoDataGroundOverlay &thisGO = static_cast<const GeoDataGroundOverlay &>(*this);
        const GeoDataGroundOverlay &otherGO = static_cast<const GeoDataGroundOverlay &>(other);

        return thisGO == otherGO;
    } else if (nodeType() == GeoDataTypes::GeoDataNetworkLinkType) {
        const GeoDataNetworkLink &thisNetLink = static_cast<const GeoDataNetworkLink &>(*this);
        const GeoDataNetworkLink &otherNetLink = static_cast<const GeoDataNetworkLink &>(other);

        return thisNetLink == otherNetLink;
    } else if (nodeType() == GeoDataTypes::GeoDataNetworkLinkControlType) {
        const GeoDataNetworkLinkControl &thisNLC = static_cast<const GeoDataNetworkLinkControl &>(*this);
        const GeoDataNetworkLinkControl &otherNLC = static_cast<const GeoDataNetworkLinkControl &>(other);

        return thisNLC == otherNLC;
    } else if (nodeType() == GeoDataTypes::GeoDataPhotoOverlayType) {
        const GeoDataPhotoOverlay &thisPO = static_cast<const GeoDataPhotoOverlay &>(*this);
        const GeoDataPhotoOverlay &otherPO = static_cast<const GeoDataPhotoOverlay &>(other);

        return thisPO == otherPO;
    } else if (nodeType() == GeoDataTypes::GeoDataPlacemarkType) {
        const GeoDataPlacemark &thisPM = static_cast<const GeoDataPlacemark &>(*this);
        const GeoDataPlacemark &otherPM = static_cast<const GeoDataPlacemark &>(other);

        return thisPM == otherPM;
    } else if (nodeType() == GeoDataTypes::GeoDataScreenOverlayType) {
        const GeoDataScreenOverlay &thisSO = static_cast<const GeoDataScreenOverlay &>(*this);
        const GeoDataScreenOverlay &otherSO = static_cast<const GeoDataScreenOverlay &>(other);

        return thisSO == otherSO;
    } else if (nodeType() == GeoDataTypes::GeoDataTourType) {
        const GeoDataTour &thisTour = static_cast<const GeoDataTour &>(*this);
        const GeoDataTour &otherTour = static_cast<const GeoDataTour &>(other);

        return thisTour == otherTour;
    }

    return false;
}

bool GeoDataFeature::equals( const GeoDataFeature &other ) const
{
    Q_D(const GeoDataFeature);
    const GeoDataFeaturePrivate* const other_d = other.d_func();

    if (!GeoDataObject::equals(other) ||
        d->m_name != other_d->m_name ||
        d->m_styleUrl != other_d->m_styleUrl ||
        d->m_popularity != other_d->m_popularity ||
        d->m_zoomLevel != other_d->m_zoomLevel ||
        d->m_visible != other_d->m_visible ||
        d->m_role != other_d->m_role ||
        d->m_extendedData != other_d->m_extendedData ||
        *style() != *other.style()) {
        return false;
    }

    if ((!d->m_styleMap && other_d->m_styleMap) ||
        (d->m_styleMap && !other_d->m_styleMap)) {
        return false;
    }

    if ((d->m_styleMap && other_d->m_styleMap) &&
        (*d->m_styleMap != *other_d->m_styleMap)) {
        return false;
    }

    if ((!d->m_featureExtendedData && other_d->m_featureExtendedData && other_d->m_featureExtendedData->m_abstractView) ||
        (d->m_featureExtendedData && d->m_featureExtendedData->m_abstractView && !other_d->m_featureExtendedData)) {
        return false;
    }

    if ((d->m_featureExtendedData && other_d->m_featureExtendedData) &&
        (*d->m_featureExtendedData != *other_d->m_featureExtendedData)) {
        return false;
    }

    return true;
}

EnumFeatureId GeoDataFeature::featureId() const
{
    Q_D(const GeoDataFeature);
    return d->featureId();
}

QString GeoDataFeature::name() const
{
    Q_D(const GeoDataFeature);
    return d->m_name;
}

void GeoDataFeature::setName( const QString &value )
{
    Q_D(GeoDataFeature);
    d->m_name = value;
}

GeoDataSnippet GeoDataFeature::snippet() const
{
    Q_D(const GeoDataFeature);
    return d->featureExtendedData().m_snippet;
}

void GeoDataFeature::setSnippet( const GeoDataSnippet &snippet )
{
    Q_D(GeoDataFeature);
    d->featureExtendedData().m_snippet = snippet;
}

QString GeoDataFeature::address() const
{
    Q_D(const GeoDataFeature);
    if (!d->m_featureExtendedData) {
        return QString();
    }

    return d->featureExtendedData().m_address;
}

void GeoDataFeature::setAddress( const QString &value)
{
    Q_D(GeoDataFeature);
    if (value.isEmpty() && !d->m_featureExtendedData) {
        return; // nothing to change
    }

    d->featureExtendedData().m_address = value;
}

QString GeoDataFeature::phoneNumber() const
{
    Q_D(const GeoDataFeature);
    if (!d->m_featureExtendedData) {
        return QString();
    }

    return d->featureExtendedData().m_phoneNumber;
}

void GeoDataFeature::setPhoneNumber( const QString &value)
{
    Q_D(GeoDataFeature);
    if (value.isEmpty() && !d->m_featureExtendedData) {
        return; // nothing to change
    }

    d->featureExtendedData().m_phoneNumber = value;
}

QString GeoDataFeature::description() const
{
    Q_D(const GeoDataFeature);
    if (!d->m_featureExtendedData) {
        return QString();
    }

    return d->featureExtendedData().m_description;
}

void GeoDataFeature::setDescription( const QString &value)
{
    Q_D(GeoDataFeature);
    if (value.isEmpty() && !d->m_featureExtendedData) {
        return; // nothing to change
    }

    d->featureExtendedData().m_description = value;
}

bool GeoDataFeature::descriptionIsCDATA() const
{
    Q_D(const GeoDataFeature);
    if (!d->m_featureExtendedData) {
        return false;
    }

    return d->featureExtendedData().m_descriptionCDATA;
}

void GeoDataFeature::setDescriptionCDATA( bool cdata )
{
    Q_D(GeoDataFeature);
    d->featureExtendedData().m_descriptionCDATA = cdata;
}

const GeoDataAbstractView* GeoDataFeature::abstractView() const
{
    Q_D(const GeoDataFeature);
    if (!d->m_featureExtendedData) {
        return nullptr;
    }

    return d->featureExtendedData().m_abstractView;
}

GeoDataAbstractView *GeoDataFeature::abstractView()
{
    // FIXME: Calling detach() doesn't help at all because the m_abstractView
    // object isn't actually copied in the Private class as well.
    // detach();

    Q_D(GeoDataFeature);
    return d->featureExtendedData().m_abstractView;
}

void GeoDataFeature::setAbstractView( GeoDataAbstractView *abstractView )
{
    Q_D(GeoDataFeature);
    if (abstractView == nullptr && !d->m_featureExtendedData) {
        return; // nothing to change
    }

    d->featureExtendedData().m_abstractView = abstractView;
}

QString GeoDataFeature::styleUrl() const
{
    Q_D(const GeoDataFeature);
    return d->m_styleUrl;
}

void GeoDataFeature::setStyleUrl( const QString &value )
{
    Q_D(GeoDataFeature);
    d->m_styleUrl = value;

    if ( value.isEmpty() ) {
        d->m_style = GeoDataStyle::Ptr();
        return;
    }

    QString styleUrl = value;
    styleUrl.remove(QLatin1Char('#'));
    GeoDataObject *object = parent();
    bool found = false;
    while ( object && !found ) {
        if (GeoDataDocument *doc = geodata_cast<GeoDataDocument>(object)) {
            GeoDataStyleMap &styleMap = doc->styleMap( styleUrl );
            const QString normalStyleUrl = styleMap.value(QStringLiteral("normal"));
            if (!normalStyleUrl.isEmpty()) {
                styleUrl = normalStyleUrl;
                styleUrl.remove(QLatin1Char('#'));
            }
            // Not calling setStyle here because we don't want
            // re-parenting of the style
            d->m_style = doc->style( styleUrl );
            found = true;
        }
        object = object->parent();
    }
}

bool GeoDataFeature::isVisible() const
{
    Q_D(const GeoDataFeature);
    return d->m_visible;
}

void GeoDataFeature::setVisible( bool value )
{
    Q_D(GeoDataFeature);
    d->m_visible = value;
}

bool GeoDataFeature::isGloballyVisible() const
{
    Q_D(const GeoDataFeature);
    if ( parent() == 0 ) {
        return d->m_visible;
    }
    const GeoDataContainer *container = static_cast<const GeoDataContainer*>(parent());
    return d->m_visible && container->isGloballyVisible();
}


const GeoDataTimeSpan &GeoDataFeature::timeSpan() const
{
    Q_D(const GeoDataFeature);
    return d->featureExtendedData().m_timeSpan;
}

GeoDataTimeSpan &GeoDataFeature::timeSpan()
{
    Q_D(GeoDataFeature);
    return d->featureExtendedData().m_timeSpan;
}

void GeoDataFeature::setTimeSpan( const GeoDataTimeSpan &timeSpan )
{
    Q_D(GeoDataFeature);
    d->featureExtendedData().m_timeSpan = timeSpan;
}

const GeoDataTimeStamp &GeoDataFeature::timeStamp() const
{
    Q_D(const GeoDataFeature);
    return d->featureExtendedData().m_timeStamp;
}

GeoDataTimeStamp &GeoDataFeature::timeStamp()
{
    Q_D(GeoDataFeature);
    return d->featureExtendedData().m_timeStamp;
}

void GeoDataFeature::setTimeStamp( const GeoDataTimeStamp &timeStamp )
{
    Q_D(GeoDataFeature);
    d->featureExtendedData().m_timeStamp = timeStamp;
}

const GeoDataExtendedData &GeoDataFeature::extendedData() const
{
    Q_D(const GeoDataFeature);
    return d->m_extendedData;
}

GeoDataStyle::ConstPtr GeoDataFeature::style() const
{
    Q_D(const GeoDataFeature);
    if (d->m_style) {
        return d->m_style;
    }

    return GeoDataFeaturePrivate::s_defaultStyle;
}

GeoDataStyle::ConstPtr GeoDataFeature::customStyle() const
{
    Q_D(const GeoDataFeature);
    return d->m_style;
}

void GeoDataFeature::setStyle( const GeoDataStyle::Ptr &style )
{
    Q_D(GeoDataFeature);
    if (style)
        style->setParent( this );
    d->m_style = style;
}

GeoDataExtendedData& GeoDataFeature::extendedData()
{
    Q_D(GeoDataFeature);
    return d->m_extendedData;
}

void GeoDataFeature::setExtendedData( const GeoDataExtendedData& extendedData )
{
    Q_D(GeoDataFeature);
    d->m_extendedData = extendedData;
}

const GeoDataRegion& GeoDataFeature::region() const
{
    Q_D(const GeoDataFeature);
    return d->featureExtendedData().m_region;
}

GeoDataRegion& GeoDataFeature::region()
{
    Q_D(GeoDataFeature);
    return d->featureExtendedData().m_region;
}

void GeoDataFeature::setRegion( const GeoDataRegion& region )
{
    Q_D(GeoDataFeature);
    d->featureExtendedData().m_region = region;
}

const QString GeoDataFeature::role() const
{
    Q_D(const GeoDataFeature);
    return d->m_role;
}

void GeoDataFeature::setRole( const QString &role )
{
    Q_D(GeoDataFeature);
    d->m_role = role;
}

const GeoDataStyleMap* GeoDataFeature::styleMap() const
{
    Q_D(const GeoDataFeature);
    return d->m_styleMap;
}

void GeoDataFeature::setStyleMap( const GeoDataStyleMap* styleMap )
{
    Q_D(GeoDataFeature);
    d->m_styleMap = styleMap;
}

int GeoDataFeature::zoomLevel() const
{
    Q_D(const GeoDataFeature);
    return d->m_zoomLevel;
}

void GeoDataFeature::setZoomLevel( int zoomLevel )
{
    Q_D(GeoDataFeature);
    d->m_zoomLevel = zoomLevel;
}

qint64 GeoDataFeature::popularity() const
{
    Q_D(const GeoDataFeature);
    return d->m_popularity;
}

void GeoDataFeature::setPopularity( qint64 popularity )
{
    Q_D(GeoDataFeature);
    d->m_popularity = popularity;
}

void GeoDataFeature::pack( QDataStream& stream ) const
{
    Q_D(const GeoDataFeature);

    GeoDataObject::pack( stream );

    stream << d->m_name;
    stream << d->featureExtendedData().m_address;
    stream << d->featureExtendedData().m_phoneNumber;
    stream << d->featureExtendedData().m_description;
    stream << d->m_visible;
//    stream << d->m_visualCategory;
    stream << d->m_role;
    stream << d->m_popularity;
    stream << d->m_zoomLevel;
}

void GeoDataFeature::unpack( QDataStream& stream )
{
    Q_D(GeoDataFeature);
    GeoDataObject::unpack( stream );

    stream >> d->m_name;
    stream >> d->featureExtendedData().m_address;
    stream >> d->featureExtendedData().m_phoneNumber;
    stream >> d->featureExtendedData().m_description;
    stream >> d->m_visible;
//    stream >> (int)d->m_visualCategory;
    stream >> d->m_role;
    stream >> d->m_popularity;
    stream >> d->m_zoomLevel;
}

}
