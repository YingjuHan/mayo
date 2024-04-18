/****************************************************************************
** Copyright (c) 2024, Fougue Ltd. <https://www.fougue.pro>
** All rights reserved.
** See license at https://github.com/fougue/mayo/blob/master/LICENSE.txt
****************************************************************************/

#include "../base/libtree.h"
#include "../base/document_ptr.h"
#include "../base/signal.h"

#include <TDF_Label.hxx>

#include <QtCore/QObject>
#include <QtQml/QJSValue>

// Notes
//     Pour accéder aux éléments de l'assemblage il faut passer par TDF_Label
//     * Dans l'environnement JS on peut employer une représentation string obtenue par CafUtils::labelTag()
//       La valeur TDF_Label d'origine est retrouvée via TDF_Tool::Label() prenant en 1er argument
//       TDF_Data. Cet objet est contenu dans TDocStd_Document (voir function GetData()). TDF_Data utilise
//       en interne une map string->TDF_Label servant de cache. La fonction TDF_Data::RegisterLabel() doit
//       doit éventuellement être appelée explicitement (à déterminer)
//     * Au lieu d'une représentation string, il doit aussi être possible d'utiliser TreeNodeId qui est
//       simplement un alias de int.
//       TreeNodeId serait employé pour le parcours de l'arborescence assemblage (dans le Document)
//       Il faudra tout de même manipuler les données XCAF via TDF_Label (voir XCaf)
//       Cette 2ème solution semble la plus appropriée
//
//     Comment représenter un object TopoDS_Shape dans l'environnement JS ?
//     * Une classe wrapper de TopoDS_Shape nommée par ex JsBRepShape héritant de QObject
//       Paraît lourd en terme d'usage du heap (allocation/destruction à chaque accès d'une shape)
//     * Voir TopExp::MapShapesAndAncestors()
//
//     I/O
//     There is currently a limitation in the "granularity" of what can be exported
//
// Fonctions :
//     JsDocument::entityCount()
//     JsDocument::entityTreeNodeId(int index) -> TreeNodeId(int)
//     JsDocument::label(TreeNodeId nodeId) -> string(taglist)
//     JsDocument::isShape(string tags) -> bool
//     JsDocument::isShapeAssembly(string tags) -> bool
//     JsDocument::isShapeReference(string tags) -> bool
//     JsDocument::isShapeSimple(string tags) -> bool
//     JsDocument::isShapeSub(string tags) -> bool
//     JsDocument::productShapeLabel(string tags) -> string(taglist)
//     JsDocument::hasShapeColor(string tags) -> bool
//     JsDocument::shapeColor(string tags) -> string("#RRGGBBAA")




namespace Mayo {

class JsApplication;

class JsDocument : public QObject {
    Q_OBJECT
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
    Q_PROPERTY(int entityCount READ entityCount NOTIFY entityCountChanged)
public:
    int id() const;

    QString name() const;
    void setName(const QString& str);

    QString filePath() const;

    int entityCount() const;
    Q_INVOKABLE void traverseModelTree(QJSValue fn);
    Q_INVOKABLE QString treeNodeName(unsigned treeNodeId) const;
    Q_INVOKABLE unsigned treeNodeParent(unsigned treeNodeId) const;
    Q_INVOKABLE bool treeNodeIsAssembly(unsigned treeNodeId) const;
    Q_INVOKABLE bool treeNodeIsInstance(unsigned treeNodeId) const;
    Q_INVOKABLE bool treeNodeIsComponent(unsigned treeNodeId) const;
    //Q_INVOKABLE unsigned treeNodeInstancePart(unsigned treeNodeId) const;
    Q_INVOKABLE QString treeNodeTag(unsigned treeNodeId) const;

signals:
    void nameChanged();
    void filePathChanged();
    void entityCountChanged();

private:
    JsDocument(const DocumentPtr& doc, JsApplication* jsApp);

    const TDF_Label& treeNodeLabel(unsigned treeNodeId) const;

    const DocumentPtr& baseDocument() const { return m_doc; }

    friend class JsApplication;
    DocumentPtr m_doc;
    ScopedSignalConnections<> m_sigConns;
};

} // namespace Mayo
