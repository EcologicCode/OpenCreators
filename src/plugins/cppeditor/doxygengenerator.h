/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include <cplusplus/Overview.h>

QT_FORWARD_DECLARE_CLASS(QTextCursor)

namespace CPlusPlus { class DeclarationAST; }
namespace CPlusPlus { class Snapshot; }
namespace Utils { class FilePath; }

namespace CppEditor::Internal {

class DoxygenGenerator
{
public:
    DoxygenGenerator();

    enum DocumentationStyle {
        JavaStyle,  ///< JavaStyle comment: /**
        QtStyle,    ///< QtStyle comment: /*!
        CppStyleA,  ///< CppStyle comment variant A: ///
        CppStyleB   ///< CppStyle comment variant B: //!
    };

    void setStyle(DocumentationStyle style);
    void setStartComment(bool start);
    void setGenerateBrief(bool gen);
    void setAddLeadingAsterisks(bool add);

    QString generate(QTextCursor cursor,
                     const CPlusPlus::Snapshot &snapshot,
                     const Utils::FilePath &documentFilePath);
    QString generate(QTextCursor cursor, CPlusPlus::DeclarationAST *decl);

private:
    QChar startMark() const;
    QChar styleMark() const;

    enum Command {
        BriefCommand,
        ParamCommand,
        ReturnCommand
    };
    static QString commandSpelling(Command command);

    void writeStart(QString *comment) const;
    void writeEnd(QString *comment) const;
    void writeContinuation(QString *comment) const;
    void writeNewLine(QString *comment) const;
    void writeCommand(QString *comment,
                      Command command,
                      const QString &commandContent = QString()) const;
    void writeBrief(QString *comment,
                    const QString &brief,
                    const QString &prefix = QString(),
                    const QString &suffix = QString());

    void assignCommentOffset(QTextCursor cursor);
    QString offsetString() const;

    bool m_addLeadingAsterisks = true;
    bool m_generateBrief = true;
    bool m_startComment = true;
    DocumentationStyle m_style = QtStyle;
    CPlusPlus::Overview m_printer;
    QString m_commentOffset;
};

} // namespace CppEditor::Internal
