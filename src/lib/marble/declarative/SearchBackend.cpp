//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
//

#include "SearchBackend.h"
#include "MarblePlacemarkModel.h"

using namespace Marble;


SearchBarBackend::SearchBarBackend(QObject *parent) :
    QObject(parent),
    m_searchManager( nullptr ),
    m_marbleQuickItem( nullptr ),
    m_placemarkModel( nullptr )
{
    m_model.setSortRole( MarblePlacemarkModel::PopularityIndexRole );
    m_model.sort(0);
    m_model.setDynamicSortFilter(true);
}

void SearchBarBackend::search(const QString &place)
{
    if (m_marbleQuickItem)
    {
        m_searchManager->searchPlacemarks(place);
    }
}

QObject * SearchBarBackend::marbleQuickItem()
{
    return m_marbleQuickItem;
}

const QObject * SearchBarBackend::marbleQuickItem() const
{
    return m_marbleQuickItem;
}

void SearchBarBackend::updateMap(int placemarkIndex)
{
    QVariant data = m_placemarkModel->data(m_placemarkModel->index(placemarkIndex), MarblePlacemarkModel::ObjectPointerRole);
    if( !data.isValid() ) {
        return;
    }
    GeoDataObject *object = qvariant_cast<GeoDataObject*>( data );
    if( object == nullptr ) {
        return;
    }
    GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>( object );
    if( placemark == nullptr ) {
        return;
    }
    m_marbleQuickItem->centerOn(*placemark, true);
}

void SearchBarBackend::setMarbleQuickItem(QObject *marbleQuickItem)
{
    MarbleQuickItem * item = qobject_cast<MarbleQuickItem*>(marbleQuickItem);
    if (m_marbleQuickItem == item)
    {
        return;
    }

    if (item)
    {
        delete m_searchManager;
        m_marbleQuickItem = item;
        m_searchManager = new SearchRunnerManager(m_marbleQuickItem->model(), this);

        connect(m_searchManager, SIGNAL(searchResultChanged(QAbstractItemModel*)),
                this, SLOT(searchFinished(QAbstractItemModel*)));


        emit marbleQuickItemChanged(marbleQuickItem);
    }
}

void SearchBarBackend::searchFinished(QAbstractItemModel *result)
{
    m_placemarkModel = qobject_cast<MarblePlacemarkModel*>(result);
    emit updateSearchResults(m_placemarkModel);
}

#include "moc_SearchBackend.cpp"
