/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
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

#include "qt5captureimagenodeinstanceserver.h"
#include "servernodeinstance.h"

#include <captureddatacommand.h>
#include <createscenecommand.h>
#include <nodeinstanceclientinterface.h>

#include <QImage>
#include <QQuickItem>
#include <QQuickView>

namespace QmlDesigner {

namespace {

QImage renderImage(ServerNodeInstance rootNodeInstance, QSize minimumSize, QSize maximumSize)
{
    rootNodeInstance.updateDirtyNodeRecursive();

    QSize previewImageSize = rootNodeInstance.boundingRect().size().toSize();
    if (previewImageSize.isEmpty()) {
        previewImageSize = minimumSize;
    } else if (previewImageSize.width() < minimumSize.width()
               || previewImageSize.height() < minimumSize.height()) {
        previewImageSize.scale(minimumSize, Qt::KeepAspectRatio);
    }

    if (previewImageSize.width() > maximumSize.width()
        || previewImageSize.height() > maximumSize.height()) {
        previewImageSize.scale(maximumSize, Qt::KeepAspectRatio);
    }

    QImage previewImage = rootNodeInstance.renderPreviewImage(previewImageSize);

    return previewImage;
}
} // namespace

void Qt5CaptureImageNodeInstanceServer::collectItemChangesAndSendChangeCommands()
{
    static bool inFunction = false;

    if (!rootNodeInstance().holdsGraphical()) {
        nodeInstanceClient()->capturedData(CapturedDataCommand{});
        return;
    }

    if (!inFunction) {
        inFunction = true;

        auto rooNodeInstance = rootNodeInstance();
        if (QQuickItem *qitem = rooNodeInstance.rootQuickItem())
            qitem->setClip(true);

        DesignerSupport::polishItems(quickWindow());

        QImage image = renderImage(rooNodeInstance, m_minimumSize, m_maximumSize);

        nodeInstanceClient()->capturedData(CapturedDataCommand{std::move(image)});

        slowDownRenderTimer();
        inFunction = false;
    }
}

void QmlDesigner::Qt5CaptureImageNodeInstanceServer::createScene(const CreateSceneCommand &command)
{
    m_minimumSize = command.captureImageMinimumSize;
    m_maximumSize = command.captureImageMaximumSize;

    Qt5PreviewNodeInstanceServer::createScene(command);
}

} // namespace
