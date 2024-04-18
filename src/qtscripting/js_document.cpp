/****************************************************************************
** Copyright (c) 2024, Fougue Ltd. <https://www.fougue.pro>
** All rights reserved.
** See license at https://github.com/fougue/mayo/blob/master/LICENSE.txt
****************************************************************************/

#include "js_document.h"

#include "../base/caf_utils.h"
#include "../base/document.h"
#include "../qtcommon/filepath_conv.h"
#include "../qtcommon/qstring_conv.h"
#include "js_application.h"

namespace Mayo {

int JsDocument::id() const
{
    return m_doc ? m_doc->identifier() : -1;
}

QString JsDocument::name() const
{
    return m_doc ? to_QString(m_doc->name()) : QString{};
}

void JsDocument::setName(const QString& str)
{
    if (m_doc)
        m_doc->setName(to_stdString(str));
}

QString JsDocument::filePath() const
{
    return m_doc ? filepathTo<QString>(m_doc->filePath()) : QString{};
}

int JsDocument::entityCount() const
{
    return m_doc ? m_doc->entityCount() : 0;
}

void JsDocument::traverseModelTree(QJSValue fn)
{
    if (!fn.isCallable())
        return;

    const Tree<TDF_Label>& modelTree = m_doc->modelTree();
    traverseTree(modelTree, [&](TreeNodeId nodeId) {
        const TreeNodeId parentNodeId = modelTree.nodeParent(nodeId);
        if (parentNodeId != 0) {
            const TDF_Label& parentNodeLabel = modelTree.nodeData(parentNodeId);
            if (XCaf::isShapeReference(parentNodeLabel))
                return; // Skip: tree node is a product(or "referred" shape)
        }

        fn.call({ QJSValue{nodeId} });
    });
}

QString JsDocument::treeNodeName(unsigned treeNodeId) const
{
    return to_QString(CafUtils::labelAttrStdName(this->treeNodeLabel(treeNodeId)));
}

unsigned JsDocument::treeNodeParent(unsigned treeNodeId) const
{
    return m_doc ? m_doc->modelTree().nodeParent(treeNodeId) : 0;
}

bool JsDocument::treeNodeIsAssembly(unsigned treeNodeId) const
{
    return XCaf::isShapeAssembly(this->treeNodeLabel(treeNodeId));
}

bool JsDocument::treeNodeIsInstance(unsigned treeNodeId) const
{
    return XCaf::isShapeReference(this->treeNodeLabel(treeNodeId));
}

bool JsDocument::treeNodeIsComponent(unsigned treeNodeId) const
{
    return XCaf::isShapeComponent(this->treeNodeLabel(treeNodeId));
}

QString JsDocument::treeNodeTag(unsigned treeNodeId) const
{
    return to_QString(CafUtils::labelTag(this->treeNodeLabel(treeNodeId)));
}

JsDocument::JsDocument(const DocumentPtr& doc, JsApplication* jsApp)
    : QObject(jsApp),
      m_doc(doc)
{
    m_sigConns
        << doc->signalNameChanged.connectSlot([=](const std::string&) { emit this->nameChanged(); })
        << doc->signalFilePathChanged.connectSlot([=](const FilePath&) { emit this->filePathChanged(); })
        << doc->signalEntityAdded.connectSlot([=](TreeNodeId) { emit this->entityCountChanged(); })
        << doc->signalEntityAboutToBeDestroyed.connectSlot([=](TreeNodeId) { emit this->entityCountChanged(); })
        ;
}

const TDF_Label& JsDocument::treeNodeLabel(unsigned treeNodeId) const
{
    static const TDF_Label nullLabel;
    return m_doc ? m_doc->modelTree().nodeData(treeNodeId) : nullLabel;
}

} // namespace Mayo
