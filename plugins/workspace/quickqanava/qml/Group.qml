/*
 Copyright (c) 2008-2018, Benoit AUTHEMAN All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the author or Destrat.io nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL AUTHOR BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//-----------------------------------------------------------------------------
// This file is a part of the QuickQanava software library. Copyright 2015 Benoit AUTHEMAN.
//
// \file	Group.qml
// \author	benoit@destrat.io
// \date	2016 03 22
//-----------------------------------------------------------------------------

import QtQuick              2.7
import QtQuick.Layouts      1.3
import QtGraphicalEffects   1.0

import workspace.quickqanava 2.0 as Qan

Qan.GroupItem {
    id: groupItem

    default property alias children : template
    container: template.content   // See qan::GroupItem::container property documentation
    onContainerChanged: {
        if (container) {
            groupItem.width = Qt.binding(function() {
                return Math.max(groupItem.minimumGroupWidth, template.content.width)
            })
            groupItem.height = Qt.binding(function() {
                return Math.max(groupItem.minimumGroupHeight, template.content.height)
            })
        }
    }

    //! Show or hide group top left label editor (default to visible).
    property alias labelEditorVisible : template.labelEditorVisible

    //! Show or hide group top left expand button (default to visible).
    property alias expandButtonVisible : template.expandButtonVisible

    Qan.RectGroupTemplate {
        id: template
        anchors.fill: parent
        groupItem: parent
        z: 1

        preferredGroupWidth: parent.preferredGroupWidth
        preferredGroupHeight: parent.preferredGroupHeight
    }

    // Emitted by qan::GroupItem when node dragging start
    onNodeDragEnter: { template.onNodeDragEnter() }
    // Emitted by qan::GroupItem when node dragging ends
    onNodeDragLeave: { template.onNodeDragLeave() }
}
