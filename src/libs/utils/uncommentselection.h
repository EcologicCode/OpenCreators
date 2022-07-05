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

#include "utils_global.h"

#include <QString>
#include <QTextCursor>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
QT_END_NAMESPACE

namespace Utils {

class MultiTextCursor;

class QTCREATOR_UTILS_EXPORT CommentDefinition
{
public:
    static CommentDefinition CppStyle;
    static CommentDefinition HashStyle;

    CommentDefinition();
    CommentDefinition(const QString &single,
                      const QString &multiStart = QString(), const QString &multiEnd = QString());

    bool isValid() const;
    bool hasSingleLineStyle() const;
    bool hasMultiLineStyle() const;

public:
    bool isAfterWhiteSpaces = false;
    QString singleLine;
    QString multiLineStart;
    QString multiLineEnd;
};

QTCREATOR_UTILS_EXPORT
QTextCursor unCommentSelection(const QTextCursor &cursor,
                               const CommentDefinition &definiton = CommentDefinition(),
                               bool preferSingleLine = false);

QTCREATOR_UTILS_EXPORT
MultiTextCursor unCommentSelection(const MultiTextCursor &cursor,
                                   const CommentDefinition &definiton = CommentDefinition(),
                                   bool preferSingleLine = false);

} // namespace Utils
