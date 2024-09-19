// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Sergey Popov <sergobot@protonmail.com>
//

#include "OsmcSymbol.h"

#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QPainter>

OsmcSymbol::OsmcSymbol(const QString &tag, int size)
    : m_wayColor(Qt::white)
    , m_backgroundColor(Qt::black)
    , m_foreground(nullptr)
    , m_foreground2(nullptr)
    , m_textColor(Qt::black)
    , m_side(size)
{
    m_backgroundTypes << "round"
                      << "circle"
                      << "frame";

    m_foregroundTypes << "dot"
                      << "bowl"
                      << "circle"
                      << "bar"
                      << "stripe"
                      << "cross"
                      << "x"
                      << "slash"
                      << "backslash"
                      << "rectangle"
                      << "rectangle_line"
                      << "triangle"
                      << "triangle_turned"
                      << "triangle_line"
                      << "diamond"
                      << "pointer"
                      << "fork"
                      << "arch"
                      << "turned_T"
                      << "L"
                      << "lower"
                      << "corner"
                      << "drop_line"
                      << "horse"
                      << "hiker";

    m_precoloredForegroundTypes << "wolfshook"
                                << "shell"
                                << "shell_modern"
                                << "ammonit"
                                << "mine"
                                << "hiker"
                                << "heart"
                                << "tower"
                                << "bridleway";

    if (parseTag(tag)) {
        render();
    }
}

OsmcSymbol::~OsmcSymbol()
{
    delete m_foreground;
    delete m_foreground2;
}

bool OsmcSymbol::parseTag(const QString &tag)
{
    QStringList parts = tag.split(':');

    if (parts.size() < 2) {
        return false;
    }

    if (m_foreground) {
        delete m_foreground;
        m_foreground = nullptr;
    }
    if (m_foreground2) {
        delete m_foreground2;
        m_foreground2 = nullptr;
    }

    // Determine way color
    if (QColor::isValidColorName(parts.at(0))) {
        m_wayColor.fromString(parts.at(0));
    } else {
        return false;
    }

    if (!parseBackground(parts.at(1))) {
        return false;
    }

    if (parts.size() == 3) {
        m_foreground = parseForeground(parts.at(2));
    } else if (parts.size() == 4) {
        if (QColor::isValidColorName(parts.at(3))) {
            m_text = parts.at(2);
            m_textColor = parts.at(3);
        } else {
            m_foreground = parseForeground(parts.at(2));
            m_foreground2 = parseForeground(parts.at(3));
        }
    } else if (parts.size() == 5) {
        m_foreground = parseForeground(parts.at(2));
        if (QColor::isValidColorName(parts.at(4))) {
            m_text = parts.at(3);
            m_textColor = parts.at(4);
        } else {
            return false;
        }
    } else if (parts.size() == 6) {
        m_foreground = parseForeground(parts.at(2));
        m_foreground2 = parseForeground(parts.at(3));
        if (QColor::isValidColorName(parts.at(5))) {
            m_text = parts.at(4);
            m_textColor.fromString(parts.at(5));
        } else {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

bool OsmcSymbol::parseBackground(const QString &bg)
{
    QString color = bg.section("_", 0, 0);
    QString type = bg.section("_", 1, -1);

    if (!QColor::isValidColorName(color)) {
        return false;
    }

    // Plain color was provided
    if (type.isEmpty()) {
        m_backgroundColor.fromString(color);
        m_backgroundType = type;
    } else if (m_backgroundTypes.contains(type)) {
        m_backgroundColor.fromString(color);
        m_backgroundType = type;
    } else {
        return false;
    }

    return true;
}

void setXMLAttribute(QDomElement &elem, const QString &tag, const QString &attr, const QString &attrValue);

QSvgRenderer *OsmcSymbol::parseForeground(const QString &fg)
{
    if (m_precoloredForegroundTypes.contains(fg)) {
        return new QSvgRenderer(QStringLiteral(":/osmc-symbols/%1.svg").arg(fg));
    }

    QString color = fg.section('_', 0, 0);
    QString type = fg.section('_', 1, -1);
    if (QColor::isValidColorName(color) && m_foregroundTypes.contains(type)) {
        // Open svg resource and load contents to QByteArray
        QFile file(QStringLiteral(":/osmc-symbols/%1.svg").arg(type));
        file.open(QIODevice::ReadOnly);
        QByteArray baData = file.readAll();

        // Load svg contents to xml document
        QDomDocument doc;
        doc.setContent(baData);

        // Recursively change color
        QDomElement rootElement = doc.documentElement();
        setXMLAttribute(rootElement, "path", "fill", color);

        // Create and return svg renderer with edited contents
        return new QSvgRenderer(doc.toByteArray());
    }

    return nullptr;
}

void OsmcSymbol::render()
{
    m_image = QImage(m_side, m_side, QImage::Format_ARGB32);
    m_image.fill(Qt::transparent);

    QPainter painter(&m_image);
    painter.setRenderHint(QPainter::Antialiasing);

    // Default size of background
    int w = m_side, h = m_side;

    // If there is some text, our background size must be recalculated
    if (!m_text.isEmpty()) {
        QFont font = painter.font();
        font.setPixelSize(int(m_side * 0.8));
        font.setBold(true);
        painter.setFont(font);
        QFontMetrics fm = QFontMetrics(font);

        h = fm.height();
        w = qMax(h, fm.horizontalAdvance(m_text));
    }

    const QRect bgRect = QRect((m_side - w) / 2, (m_side - h) / 2, w, h);

    // Draw symbol's background
    if (m_backgroundType.isEmpty()) {
        painter.fillRect(bgRect, m_backgroundColor);
    } else if (m_backgroundType == "round") {
        painter.setBrush(m_backgroundColor);
        painter.setPen(m_backgroundColor);
        painter.drawEllipse(bgRect);
    } else if (m_backgroundType == "circle") {
        painter.setBrush(Qt::white);
        painter.setPen(QPen(m_backgroundColor, m_side / 10));
        painter.drawEllipse(bgRect);
    } else if (m_backgroundType == "frame") {
        painter.setPen(QPen(m_backgroundColor, m_side / 10));
        painter.fillRect(bgRect, Qt::white);
        painter.drawRect(bgRect);
    }

    QPixmap foregrounds(bgRect.size());
    foregrounds.fill(Qt::transparent);
    QPainter fgPainter(&foregrounds);
    m_foreground ? m_foreground->render(&fgPainter) : void();
    m_foreground2 ? m_foreground2->render(&fgPainter) : void();
    painter.drawPixmap(bgRect, foregrounds);

    if (!m_text.isEmpty()) {
        // Draw text with provided color
        painter.setPen(m_textColor);
        painter.drawText(bgRect, Qt::AlignCenter, m_text);
    }

    painter.end();
}

QImage OsmcSymbol::icon() const
{
    return m_image;
}

QColor OsmcSymbol::wayColor() const
{
    return m_wayColor;
}

void setXMLAttribute(QDomElement &elem, const QString &tag, const QString &attr, const QString &attrValue)
{
    // If elem's tag is equal to the provided one then overwrite desired attribute
    if (elem.tagName() == tag) {
        elem.setAttribute(attr, attrValue);
    }

    // Do the same for all the child nodes
    for (int i = 0; i < elem.childNodes().count(); ++i) {
        if (!elem.childNodes().at(i).isElement()) {
            continue;
        }

        QDomElement child = elem.childNodes().at(i).toElement();
        setXMLAttribute(child, tag, attr, attrValue);
    }
}
