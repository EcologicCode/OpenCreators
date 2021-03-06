/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qbs.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <language/scriptengine.h>
#include <logging/translator.h>
#include <tools/hostosinfo.h>

#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qobject.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qvariant.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QtCore5Compat/qtextcodec.h>
#else
#include <QtCore/qtextcodec.h>
#endif

#include <QtScript/qscriptable.h>
#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptvalue.h>

namespace qbs {
namespace Internal {

class TextFile : public QObject, public QScriptable, public ResourceAcquiringScriptObject
{
    Q_OBJECT
public:
    enum OpenMode
    {
        ReadOnly = 1,
        WriteOnly = 2,
        ReadWrite = ReadOnly | WriteOnly,
        Append = 4
    };
    Q_ENUM(OpenMode)

    static QScriptValue ctor(QScriptContext *context, QScriptEngine *engine);

    Q_INVOKABLE void close();
    Q_INVOKABLE QString filePath();
    Q_INVOKABLE void setCodec(const QString &codec);
    Q_INVOKABLE QString readLine();
    Q_INVOKABLE QString readAll();
    Q_INVOKABLE bool atEof() const;
    Q_INVOKABLE void truncate();
    Q_INVOKABLE void write(const QString &str);
    Q_INVOKABLE void writeLine(const QString &str);

private:
    TextFile(QScriptContext *context, const QString &filePath, OpenMode mode = ReadOnly,
             const QString &codec = QLatin1String("UTF-8"));

    bool checkForClosed() const;

    // ResourceAcquiringScriptObject implementation
    void releaseResources() override;

    std::unique_ptr<QFile> m_file;
    QTextCodec *m_codec = nullptr;
};

QScriptValue TextFile::ctor(QScriptContext *context, QScriptEngine *engine)
{
    TextFile *t;
    switch (context->argumentCount()) {
    case 0:
        return context->throwError(Tr::tr("TextFile constructor needs path of file to be opened."));
    case 1:
        t = new TextFile(context, context->argument(0).toString());
        break;
    case 2:
        t = new TextFile(context,
                         context->argument(0).toString(),
                         static_cast<OpenMode>(context->argument(1).toInt32())
                         );
        break;
    case 3:
        t = new TextFile(context,
                         context->argument(0).toString(),
                         static_cast<OpenMode>(context->argument(1).toInt32()),
                         context->argument(2).toString()
                         );
        break;
    default:
        return context->throwError(Tr::tr("TextFile constructor takes at most three parameters."));
    }

    const auto se = static_cast<ScriptEngine *>(engine);
    se->addResourceAcquiringScriptObject(t);
    const DubiousContextList dubiousContexts({
            DubiousContext(EvalContext::PropertyEvaluation, DubiousContext::SuggestMoving)
    });
    se->checkContext(QStringLiteral("qbs.TextFile"), dubiousContexts);
    se->setUsesIo();

    return engine->newQObject(t, QScriptEngine::QtOwnership);
}

TextFile::TextFile(QScriptContext *context, const QString &filePath, OpenMode mode,
                   const QString &codec)
{
    Q_UNUSED(codec)
    Q_ASSERT(thisObject().engine() == engine());

    m_file = std::make_unique<QFile>(filePath);
    const auto newCodec = QTextCodec::codecForName(qPrintable(codec));
    m_codec = newCodec ? newCodec : QTextCodec::codecForName("UTF-8");
    QIODevice::OpenMode m = QIODevice::NotOpen;
    if (mode & ReadOnly)
        m |= QIODevice::ReadOnly;
    if (mode & WriteOnly)
        m |= QIODevice::WriteOnly;
    if (mode & Append)
        m |= QIODevice::Append;
    m |= QIODevice::Text;
    if (Q_UNLIKELY(!m_file->open(m))) {
        context->throwError(Tr::tr("Unable to open file '%1': %2")
                            .arg(filePath, m_file->errorString()));
        m_file.reset();
    }
}

void TextFile::close()
{
    if (checkForClosed())
        return;
    m_file->close();
    m_file.reset();
}

QString TextFile::filePath()
{
    if (checkForClosed())
        return {};
    return QFileInfo(*m_file).absoluteFilePath();
}

void TextFile::setCodec(const QString &codec)
{
    if (checkForClosed())
        return;
    const auto newCodec = QTextCodec::codecForName(qPrintable(codec));
    if (newCodec)
        m_codec = newCodec;
}

QString TextFile::readLine()
{
    if (checkForClosed())
        return {};
    auto result = m_codec->toUnicode(m_file->readLine());
    if (!result.isEmpty() && result.back() == QLatin1Char('\n'))
        result.chop(1);
    return result;
}

QString TextFile::readAll()
{
    if (checkForClosed())
        return {};
    return m_codec->toUnicode(m_file->readAll());
}

bool TextFile::atEof() const
{
    if (checkForClosed())
        return true;
    return m_file->atEnd();
}

void TextFile::truncate()
{
    if (checkForClosed())
        return;
    m_file->resize(0);
}

void TextFile::write(const QString &str)
{
    if (checkForClosed())
        return;
    m_file->write(m_codec->fromUnicode(str));
}

void TextFile::writeLine(const QString &str)
{
    if (checkForClosed())
        return;
    m_file->write(m_codec->fromUnicode(str));
    m_file->putChar('\n');
}

bool TextFile::checkForClosed() const
{
    if (m_file)
        return false;
    QScriptContext *ctx = context();
    if (ctx)
        ctx->throwError(Tr::tr("Access to TextFile object that was already closed."));
    return true;
}

void TextFile::releaseResources()
{
    close();
    deleteLater();
}

} // namespace Internal
} // namespace qbs

void initializeJsExtensionTextFile(QScriptValue extensionObject)
{
    using namespace qbs::Internal;
    QScriptEngine *engine = extensionObject.engine();
    QScriptValue obj = engine->newQMetaObject(&TextFile::staticMetaObject,
                                              engine->newFunction(&TextFile::ctor));
    extensionObject.setProperty(QStringLiteral("TextFile"), obj);
}

Q_DECLARE_METATYPE(qbs::Internal::TextFile *)

#include "textfile.moc"
