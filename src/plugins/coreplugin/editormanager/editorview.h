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

#include <utils/dropsupport.h>
#include <utils/fileutils.h>
#include <utils/id.h>

#include <QMap>
#include <QList>
#include <QString>
#include <QPointer>
#include <QVariant>

#include <QIcon>
#include <QWidget>

#include <functional>

QT_BEGIN_NAMESPACE
class QFrame;
class QLabel;
class QMenu;
class QSplitter;
class QStackedLayout;
class QStackedWidget;
class QToolButton;
QT_END_NAMESPACE

namespace Utils {
class InfoBarDisplay;
}

namespace Core {
class IDocument;
class IEditor;
class EditorToolBar;

namespace Internal {

struct EditLocation {
    QPointer<IDocument> document;
    Utils::FilePath filePath;
    Utils::Id id;
    QVariant state;
};

class SplitterOrView;

class EditorView : public QWidget
{
    Q_OBJECT

public:
    explicit EditorView(SplitterOrView *parentSplitterOrView, QWidget *parent = nullptr);
    ~EditorView() override;

    SplitterOrView *parentSplitterOrView() const;
    EditorView *findNextView() const;
    EditorView *findPreviousView() const;

    int editorCount() const;
    void addEditor(IEditor *editor);
    void removeEditor(IEditor *editor);
    IEditor *currentEditor() const;
    void setCurrentEditor(IEditor *editor);

    bool hasEditor(IEditor *editor) const;

    QList<IEditor *> editors() const;
    IEditor *editorForDocument(const IDocument *document) const;

    void showEditorStatusBar(const QString &id,
                           const QString &infoText,
                           const QString &buttonText,
                           QObject *object, const std::function<void()> &function);
    void hideEditorStatusBar(const QString &id);
    void setCloseSplitEnabled(bool enable);
    void setCloseSplitIcon(const QIcon &icon);

    static void updateEditorHistory(IEditor *editor, QList<EditLocation> &history);

signals:
    void currentEditorChanged(Core::IEditor *editor);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;
    void focusInEvent(QFocusEvent *) override;

private:
    friend class SplitterOrView; // for setParentSplitterOrView

    void closeCurrentEditor();
    void listSelectionActivated(int index);
    void splitHorizontally();
    void splitVertically();
    void splitNewWindow();
    void closeSplit();
    void openDroppedFiles(const QList<Utils::DropSupport::FileSpec> &files);

    void setParentSplitterOrView(SplitterOrView *splitterOrView);

    void fillListContextMenu(QMenu *menu) const;
    void updateNavigatorActions();
    void updateToolBar(IEditor *editor);
    void checkProjectLoaded(IEditor *editor);

    SplitterOrView *m_parentSplitterOrView;
    EditorToolBar *m_toolBar;

    QStackedWidget *m_container;
    Utils::InfoBarDisplay *m_infoBarDisplay;
    QString m_statusWidgetId;
    QFrame *m_statusHLine;
    QFrame *m_statusWidget;
    QLabel *m_statusWidgetLabel;
    QToolButton *m_statusWidgetButton;
    QList<IEditor *> m_editors;
    QMap<QWidget *, IEditor *> m_widgetEditorMap;
    QLabel *m_emptyViewLabel;

    QList<EditLocation> m_navigationHistory;
    QList<EditLocation> m_editorHistory;
    int m_currentNavigationHistoryPosition = 0;
    void updateCurrentPositionInNavigationHistory();

public:
    inline bool canGoForward() const { return m_currentNavigationHistoryPosition < m_navigationHistory.size()-1; }
    inline bool canGoBack() const { return m_currentNavigationHistoryPosition > 0; }

public slots:
    void goBackInNavigationHistory();
    void goForwardInNavigationHistory();

public:
    void goToEditLocation(const EditLocation &location);

    void addCurrentPositionToNavigationHistory(const QByteArray &saveState = QByteArray());
    void cutForwardNavigationHistory();

    inline QList<EditLocation> editorHistory() const { return m_editorHistory; }

    void copyNavigationHistoryFrom(EditorView* other);
    void updateEditorHistory(IEditor *editor);
};

class SplitterOrView  : public QWidget
{
    Q_OBJECT
public:
    explicit SplitterOrView(IEditor *editor = nullptr);
    explicit SplitterOrView(EditorView *view);
    ~SplitterOrView() override;

    void split(Qt::Orientation orientation, bool activateView = true);
    void unsplit();

    inline bool isView() const { return m_view != nullptr; }
    inline bool isSplitter() const { return m_splitter != nullptr; }

    inline IEditor *editor() const { return m_view ? m_view->currentEditor() : nullptr; }
    inline QList<IEditor *> editors() const { return m_view ? m_view->editors() : QList<IEditor*>(); }
    inline bool hasEditor(IEditor *editor) const { return m_view && m_view->hasEditor(editor); }
    inline bool hasEditors() const { return m_view && m_view->editorCount() != 0; }
    inline EditorView *view() const { return m_view; }
    inline QSplitter *splitter() const { return m_splitter; }
    QSplitter *takeSplitter();
    EditorView *takeView();

    QByteArray saveState() const;
    void restoreState(const QByteArray &);

    EditorView *findFirstView();
    EditorView *findLastView();
    SplitterOrView *findParentSplitter() const;

    QSize sizeHint() const override { return minimumSizeHint(); }
    QSize minimumSizeHint() const override;

    void unsplitAll();

signals:
    void splitStateChanged();

private:
    const QList<IEditor *> unsplitAll_helper();
    QStackedLayout *m_layout;
    EditorView *m_view;
    QSplitter *m_splitter;
};

}
}