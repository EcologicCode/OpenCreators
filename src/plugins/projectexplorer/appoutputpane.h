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

#include "projectexplorersettings.h"

#include <coreplugin/ioutputpane.h>
#include <coreplugin/dialogs/ioptionspage.h>

#include <utils/outputformat.h>

#include <QPointer>
#include <QVector>

QT_BEGIN_NAMESPACE
class QTabWidget;
class QToolButton;
class QAction;
class QPoint;
QT_END_NAMESPACE

namespace Core { class OutputWindow; }

namespace ProjectExplorer {

class RunControl;
class Project;

namespace Internal {

class ShowOutputTaskHandler;
class TabWidget;

class AppOutputPane : public Core::IOutputPane
{
    Q_OBJECT

public:
    enum CloseTabMode {
        CloseTabNoPrompt,
        CloseTabWithPrompt
    };

    AppOutputPane();
    ~AppOutputPane() override;

    QWidget *outputWidget(QWidget *) override;
    QList<QWidget *> toolBarWidgets() const override;
    QString displayName() const override;
    int priorityInStatusBar() const override;
    void clearContents() override;
    bool canFocus() const override;
    bool hasFocus() const override;
    void setFocus() override;

    bool canNext() const override;
    bool canPrevious() const override;
    void goToNext() override;
    void goToPrev() override;
    bool canNavigate() const override;

    void createNewOutputWindow(RunControl *rc);
    void showTabFor(RunControl *rc);
    void setBehaviorOnOutput(RunControl *rc, AppOutputPaneMode mode);

    bool aboutToClose() const;
    void closeTabs(CloseTabMode mode);

    QList<RunControl *> allRunControls() const;

    // ApplicationOutput specifics
    void projectRemoved();

    void appendMessage(ProjectExplorer::RunControl *rc, const QString &out,
                       Utils::OutputFormat format);

    const AppOutputSettings &settings() const { return m_settings; }
    void setSettings(const AppOutputSettings &settings);

private:
    void reRunRunControl();
    void stopRunControl();
    void attachToRunControl();
    void tabChanged(int);
    void contextMenuRequested(const QPoint &pos, int index);
    void slotRunControlChanged();
    void slotRunControlFinished();
    void slotRunControlFinished2(ProjectExplorer::RunControl *sender);

    void aboutToUnloadSession();
    void updateFromSettings();
    void enableDefaultButtons();

    void zoomIn(int range);
    void zoomOut(int range);
    void resetZoom();

    void enableButtons(const RunControl *rc);

    class RunControlTab {
    public:
        explicit RunControlTab(RunControl *runControl = nullptr,
                               Core::OutputWindow *window = nullptr);
        QPointer<RunControl> runControl;
        QPointer<Core::OutputWindow> window;
        AppOutputPaneMode behaviorOnOutput = AppOutputPaneMode::FlashOnOutput;
    };

    void closeTab(int index, CloseTabMode cm = CloseTabWithPrompt);
    bool optionallyPromptToStop(RunControl *runControl);

    int indexOf(const RunControl *) const;
    int indexOf(const QWidget *outputWindow) const;
    int currentIndex() const;
    RunControl *currentRunControl() const;
    int tabWidgetIndexOf(int runControlIndex) const;
    void handleOldOutput(Core::OutputWindow *window) const;
    void updateCloseActions();
    void updateFilter() override;
    const QList<Core::OutputWindow *> outputWindows() const override;
    void ensureWindowVisible(Core::OutputWindow *ow) override;

    void loadSettings();
    void storeSettings() const;

    QWidget *m_mainWidget;
    TabWidget *m_tabWidget;
    QVector<RunControlTab> m_runControlTabs;
    int m_runControlCount = 0;
    QAction *m_stopAction;
    QAction *m_closeCurrentTabAction;
    QAction *m_closeAllTabsAction;
    QAction *m_closeOtherTabsAction;
    QToolButton *m_reRunButton;
    QToolButton *m_stopButton;
    QToolButton *m_attachButton;
    QToolButton * const m_settingsButton;
    QWidget *m_formatterWidget;
    ShowOutputTaskHandler * const m_handler;
    AppOutputSettings m_settings;
};

class AppOutputSettingsPage final : public Core::IOptionsPage
{
public:
    AppOutputSettingsPage();
};

} // namespace Internal
} // namespace ProjectExplorer
