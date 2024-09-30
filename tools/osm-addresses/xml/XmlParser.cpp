// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#include "XmlParser.h"

#include <QDebug>

namespace Marble
{

XmlParser::XmlParser(QObject *parent)
    : OsmParser(parent)
{
    // nothing to do
}

bool XmlParser::parse(const QFileInfo &content, int, bool &needAnotherPass)
{
    needAnotherPass = false;

    QXmlSimpleReader xmlReader;
    xmlReader.setContentHandler(this);
    xmlReader.setErrorHandler(this);

    QFile file(content.absoluteFilePath());
    QXmlInputSource *source = new QXmlInputSource(&file);

    if (!xmlReader.parse(source)) {
        qCritical() << "Failed to parse " << content.absoluteFilePath();
        return false;
    }

    return true;
}

bool XmlParser::startElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString &qName, const QXmlAttributes &atts)
{
    if (qName == QLatin1StringView("node")) {
        m_node = Node();
        m_id = atts.value("id").toInt();
        m_node.lon = atts.value("lon").toFloat();
        m_node.lat = atts.value("lat").toFloat();
        m_element = NodeType;
    } else if (qName == QLatin1StringView("way")) {
        m_id = atts.value("id").toInt();
        m_way = Way();
        m_element = WayType;
    } else if (qName == QLatin1StringView("nd")) {
        m_way.nodes.push_back(atts.value("ref").toInt());
    } else if (qName == QLatin1StringView("relation")) {
        m_id = atts.value("id").toInt();
        m_relation = Relation();
        m_relation.nodes.clear();
        m_element = RelationType;
    } else if (qName == QLatin1StringView("member")) {
        if (atts.value("type") == QLatin1StringView("node")) {
            m_relation.nodes.push_back(atts.value("ref").toInt());
        } else if (atts.value("type") == QLatin1StringView("way")) {
            RelationRole role = None;
            if (atts.value("role") == QLatin1StringView("outer"))
                role = Outer;
            if (atts.value("role") == QLatin1StringView("inner"))
                role = Inner;
            m_relation.ways.push_back(QPair<int, RelationRole>(atts.value("ref").toInt(), role));
        } else if (atts.value("type") == QLatin1StringView("relation")) {
            m_relation.relations.push_back(atts.value("ref").toInt());
        } else {
            qDebug() << "Unknown relation member type " << atts.value("type");
        }
    } else if (qName == QLatin1StringView("tag") && m_element == RelationType) {
        if (atts.value("k") == QLatin1StringView("boundary") && atts.value("v") == QLatin1StringView("administrative")) {
            m_relation.isAdministrativeBoundary = true;
        } else if (atts.value("k") == QLatin1StringView("admin_level")) {
            m_relation.adminLevel = atts.value("v").toInt();
        } else if (atts.value("k") == QLatin1StringView("name")) {
            m_relation.name = atts.value("v");
        } else if (atts.value("k") == QLatin1StringView("type") && atts.value("v") == QLatin1StringView("multipolygon")) {
            m_relation.isMultipolygon = true;
        }
    } else if (qName == QLatin1StringView("tag") && m_element == WayType) {
        QString const key = atts.value("k");
        QString const value = atts.value("v");
        if (key == QLatin1StringView("name")) {
            m_way.name = value;
        } else if (key == QLatin1StringView("addr:street")) {
            m_way.street = value;
            m_way.save = true;
        } else if (key == QLatin1StringView("addr:housenumber")) {
            m_way.houseNumber = value;
            m_way.save = true;
        } else if (key == QLatin1StringView("addr:city")) {
            m_way.city = value;
            m_way.save = true;
        } else if (key == QLatin1StringView("building") && value == QLatin1StringView("yes")) {
            m_way.isBuilding = true;
        } else {
            if (shouldSave(WayType, key, value)) {
                m_way.save = true;
            }
            setCategory(m_way, key, value);
        }
    } else if (qName == QLatin1StringView("tag") && m_element == NodeType) {
        QString const key = atts.value("k");
        QString const value = atts.value("v");
        if (key == QLatin1StringView("name")) {
            m_node.name = value;
        } else if (key == QLatin1StringView("addr:street")) {
            m_node.street = value;
            m_node.save = true;
        } else if (key == QLatin1StringView("addr:housenumber")) {
            m_node.houseNumber = value;
            m_node.save = true;
        } else if (key == QLatin1StringView("addr:city")) {
            m_node.city = value;
            m_node.save = true;
        } else {
            if (shouldSave(NodeType, key, value)) {
                m_node.save = true;
            }
            setCategory(m_node, key, value);
        }
    }

    return true;
}

bool XmlParser::endElement(const QString & /*namespaceURI*/, const QString & /*localName*/, const QString &qName)
{
    if (qName == QLatin1StringView("node")) {
        m_nodes[m_id] = m_node;
    } else if (qName == QLatin1StringView("way")) {
        m_ways[m_id] = m_way;
    } else if (qName == QLatin1StringView("relation")) {
        m_relations[m_id] = m_relation;
    }

    return true;
}

}

#include "moc_XmlParser.cpp"
