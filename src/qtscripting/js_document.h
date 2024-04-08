/****************************************************************************
** Copyright (c) 2024, Fougue Ltd. <https://www.fougue.pro>
** All rights reserved.
** See license at https://github.com/fougue/mayo/blob/master/LICENSE.txt
****************************************************************************/

#include "../base/document_ptr.h"
#include "../base/signal.h"

#include <QtCore/QObject>

namespace Mayo {

class JsApplication;

class JsDocument : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
    Q_PROPERTY(int entityCount READ entityCount NOTIFY entityCountChanged)
public:
    QString name() const;
    void setName(const QString& str);

    QString filePath() const;

    int entityCount() const;

signals:
    void nameChanged();
    void filePathChanged();
    void entityCountChanged();

private:
    JsDocument(const DocumentPtr& doc, JsApplication* jsApp);

    const DocumentPtr& baseDocument() const { return m_doc; }

    friend class JsApplication;
    DocumentPtr m_doc;
    ScopedSignalConnections<> m_sigConns;
};

} // namespace Mayo
