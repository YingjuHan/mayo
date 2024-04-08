/****************************************************************************
** Copyright (c) 2024, Fougue Ltd. <https://www.fougue.pro>
** All rights reserved.
** See license at https://github.com/fougue/mayo/blob/master/LICENSE.txt
****************************************************************************/

#include "js_application.h"
#include "js_document.h"

#include "../base/application.h"
#include "../base/cpp_utils.h"
#include "../qtcommon/filepath_conv.h"
#include "../qtcommon/qstring_conv.h"
#include <common/mayo_version.h>

namespace Mayo {

JsApplication::JsApplication(const ApplicationPtr& app, QObject* parent)
    : QObject(parent),
      m_app(app)
{
    if (app) {
        m_conn = app->signalDocumentAboutToClose.connectSlot(&JsApplication::onDocumentAboutToClose, this);
        for (Application::DocumentIterator itDoc(app); itDoc.hasNext(); itDoc.next()) {
            auto jsDoc = new JsDocument(itDoc.current(), this);
            m_vecJsDoc.push_back(jsDoc);
            m_mapIdToJsDocument.insert({ itDoc.current()->identifier(), jsDoc });
        }
    }
}

QString JsApplication::versionString() const
{
    return to_QString(Mayo::strVersion);
}

int JsApplication::documentCount() const
{
    return m_app ? m_app->documentCount() : 0;
}

QObject* JsApplication::newDocument()
{
    if (!m_app)
        return nullptr;

    auto doc = m_app->newDocument();
    auto jsDoc = new JsDocument(doc, this);
    m_vecJsDoc.push_back(jsDoc);
    m_mapIdToJsDocument.insert({ doc->identifier(), jsDoc });
    emit this->documentAdded(jsDoc);
    emit this->documentCountChanged();
    return jsDoc;
}

QObject* JsApplication::documentAt(int docIndex) const
{
    if (0 <= docIndex && docIndex < m_vecJsDoc.size())
        return m_vecJsDoc.at(docIndex);
    else
        return nullptr;
}

QObject* JsApplication::findDocumentByLocation(const QString& location) const
{
    if (!m_app)
        return nullptr;

    auto doc = m_app->findDocumentByLocation(filepathFrom(location));
    return CppUtils::findValue(doc ? doc->identifier() : -1, m_mapIdToJsDocument);
}

int JsApplication::findIndexOfDocument(QObject* doc) const
{
    auto jsDoc = qobject_cast<JsDocument*>(doc);
    if (!m_app || !jsDoc)
        return -1;

    return m_app->findIndexOfDocument(jsDoc->baseDocument());
}

void JsApplication::closeDocument(QObject* doc)
{
    auto jsDoc = qobject_cast<JsDocument*>(doc);
    if (m_app && jsDoc)
        m_app->closeDocument(jsDoc->baseDocument());
}

void JsApplication::onDocumentAboutToClose(const DocumentPtr& doc)
{
    auto jsDoc = CppUtils::findValue(doc ? doc->identifier() : -1, m_mapIdToJsDocument);
    if (jsDoc) {
        emit this->documentAboutToClose(jsDoc);
        m_mapIdToJsDocument.erase(doc->identifier());
        m_vecJsDoc.erase(std::find(m_vecJsDoc.begin(), m_vecJsDoc.end(), jsDoc));
        jsDoc->deleteLater();
        emit this->documentCountChanged();
    }
}

} // namespace Mayo
