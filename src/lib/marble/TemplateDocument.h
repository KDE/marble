// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef MARBLE_TEMPLATEDOCUMENT_H
#define MARBLE_TEMPLATEDOCUMENT_H

#include <QtGlobal>

class QString;

namespace Marble
{

class TemplateDocumentPrivate;

/**
 * @brief The Template Document
 *
 * The class represents a template engine for
 * Marble HTML/plain text. It allows to template big
 * documents with variables like %variable_name% or
 * to make includes like %!{include_name}%
 *
 * "Include" is a special type of template variables which
 * allows to include extra already templated text into the
 * template. E.g. %!{bootstrap}% will include bootstrap CSS
 * files into html file, where this include is called
 *
 * @see TemplateDocument()
 *
 */
class TemplateDocument
{
public:
    TemplateDocument();
    explicit TemplateDocument(const QString &templateText);
    ~TemplateDocument();

    /**
     * @brief Returns the current template value of @p key
     * @param key template key (\<here\>)
     * @return value of the template
     */
    QString value(const QString &key) const;

    /**
     * @brief Change set template value into new one
     * @param key template key
     * @param value template value
     */
    void setValue(const QString &key, const QString &value);

    /**
     * @brief Set template text
     * @param newTemplateText new template text
     */
    void setTemplate(const QString &newTemplateText);

    /**
     * @brief Indexator for template values
     * @param key template value's index
     * @return reference for the item
     */
    QString &operator[](const QString &key);

    /**
     * @brief Final proceed text
     *
     * @return ready text with all variables and includes processed
     */
    QString finalText() const;

private:
    Q_DISABLE_COPY(TemplateDocument)
    TemplateDocumentPrivate *const d;
};

} // namespace Marble

#endif // MARBLE_TEMPLATEDOCUMENT_H
