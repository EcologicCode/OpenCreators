/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
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

import QtQuick 6.0
import QtQuick3D 6.0

IconGizmo {
    id: cameraGizmo

    property Model frustumModel: null
    property bool globalShowFrustum: false

    iconSource: "qrc:///qtquickplugin/mockfiles/images/editor_camera.png"

    function connectFrustum(frustum)
    {
        frustumModel = frustum;

        frustum.selected = selected;
        frustum.selected = Qt.binding(function() {return selected;});

        frustum.scene = scene;
        frustum.scene = Qt.binding(function() {return scene;});

        frustum.targetNode = targetNode;
        frustum.targetNode = Qt.binding(function() {return targetNode;});

        frustum.visible = (canBeVisible && globalShowFrustum)
                          || (targetNode && selected && activeScene === scene);
        frustum.visible = Qt.binding(function() {
            return (canBeVisible && globalShowFrustum)
                   || (targetNode && selected && activeScene === scene);
        });
    }

    onActiveSceneChanged: {
        if (frustumModel && activeScene == scene)
            frustumModel.updateGeometry();
    }
}
