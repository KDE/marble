//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "TemplateDocument.h"

#include <QMap>
#include <QString>
#include <QFile>
#include <QRegExp>

#include "MarbleDebug.h"

namespace Marble
{

class TemplateDocumentPrivate
{
public:
    TemplateDocumentPrivate()
    {
    }
    QString templateText;
    QMap<QString, QString> templateEntries;
    static void processTemplateIncludes(QString &input);
};

void TemplateDocumentPrivate::processTemplateIncludes(QString &input)
{
    QRegExp rx("%!\\{([^}]*)\\}%");

    QStringList includes;
    int pos = 0;

    while ((pos = rx.indexIn(input, pos)) != -1) {
        includes << rx.cap(1);
        pos += rx.matchedLength();
    }

    foreach (const QString &include, includes) {
        QFile includeFile(QLatin1String(":/htmlfeatures/includes/") + include + QLatin1String(".inc"));
        if (includeFile.open(QIODevice::ReadOnly)) {
            input.replace(QLatin1String("%!{") + include + QLatin1String("}%"), includeFile.readAll());
        } else {
            mDebug() << "[WARNING] Can't process template include" << include;
        }
    }
}

TemplateDocument::TemplateDocument() :
    d(new TemplateDocumentPrivate)
{
}

TemplateDocument::TemplateDocument(const QString &templateText) :
    d(new TemplateDocumentPrivate)
{
    setTemplate(templateText);
}

TemplateDocument::~TemplateDocument()
{
    delete d;
}

void TemplateDocument::setTemplate(const QString &newTemplateText)
{
    d->templateText = newTemplateText;
}

QString TemplateDocument::value(const QString &key) const
{
    return d->templateEntries[key];
}

void TemplateDocument::setValue(const QString &key, const QString &value)
{
    d->templateEntries[key] = value;
}

QString& TemplateDocument::operator[](const QString &key)
{
    return d->templateEntries[key];
}

QString TemplateDocument::finalText() const
{
    QString ready = d->templateText;
    typedef QMap<QString, QString>::ConstIterator ConstIterator;
    ConstIterator end = d->templateEntries.constEnd();
    for (ConstIterator i = d->templateEntries.constBegin(); i != end; i++) {
        ready.replace(QLatin1Char('%') + i.key() + QLatin1Char('%'), i.value());
    }
    d->processTemplateIncludes(ready);
    return ready;
}

} // namespace Marble
