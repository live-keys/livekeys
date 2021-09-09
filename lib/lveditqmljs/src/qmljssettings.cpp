/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "qmljssettings.h"
#include "qmljshighlighter_p.h"

namespace lv{

const std::string QmlJsSettings::formatStrings[] = {
    "Text",
    "Comment",
    "Number",
    "String",
    "Operator",
    "Identifier",
    "Keyword",
    "BuiltIn",
    "QmlProperty",
    "QmlType",
    "QmlRuntimeBoundProperty",
    "QmlRuntimeModifiedValue",
    "QmlEdit"
};

QmlJsSettings::QmlJsSettings(EditorSettings *parent)
    : QObject(parent)
{
    if ( parent )
        connect(parent, &EditorSettings::refresh, this, &QmlJsSettings::__refresh);

    m_formats[QmlJsSettings::Text]        = createFormat("#fff");
    m_formats[QmlJsSettings::Comment]     = createFormat("#56748a");
    m_formats[QmlJsSettings::Number]      = createFormat("#ba761d");
    m_formats[QmlJsSettings::String]      = createFormat("#86a930");
    m_formats[QmlJsSettings::Operator]    = createFormat("#bc900c");
    m_formats[QmlJsSettings::Identifier]  = createFormat("#93672f");
    m_formats[QmlJsSettings::Keyword]     = createFormat("#a0a000");
    m_formats[QmlJsSettings::BuiltIn]     = createFormat("#93672f");

    m_formats[QmlJsSettings::QmlProperty] = createFormat("#ccc");
    m_formats[QmlJsSettings::QmlType]     = createFormat("#0080a0");
    m_formats[QmlJsSettings::QmlRuntimeBoundProperty] = createFormat("#26539f");
    m_formats[QmlJsSettings::QmlRuntimeModifiedValue] = createFormat("#0080a0");
    m_formats[QmlJsSettings::QmlEdit] = createFormat("#fff", "#0b273f");

    m_defaultPalettes["qml/double"] = "SliderPalette";
    m_defaultPalettes["qml/int"]    = "IntSliderPalette";
    m_defaultPalettes["qml/string"] = "TextPalette";
    m_defaultPalettes["qml/color"] = "ColorPalette";
    m_defaultPalettes["qml/base#Exec"] = "ExecPropertiesPalette";
    m_defaultPalettes["qml/base#StreamValue"] = "StreamValuePalette";
    m_defaultPalettes["qml/base#StreamLog"] = "StreamLogPropertiesPalette";
    m_defaultPalettes["qml/base#Act.returns"] = "ActReturnsPalette";
    m_defaultPalettes["qml/base#Arrange"] = "ArrangePalette";
    m_defaultPalettes["qml/lcvcore#VideoDecoderView"] = "VideoDecoderViewPalette";
    m_defaultPalettes["qml/lcvcore#VideoFile"] = "VideoFilePropertiesPalette";
    m_defaultPalettes["qml/lcvcore#VideoFile.streamType"] = "VideoFileStreamTypePalette";
    m_defaultPalettes["qml/lcvcore#GrayscaleView"] = "GrayscaleViewPalette";
    m_defaultPalettes["qml/QtQuick#Rectangle"] = "RectangleSizePalette";
    m_defaultPalettes["qml/timeline#Timeline"] = "TimelinePalette";
    m_defaultPalettes["qml/table#Table"] = "TablePalette";
    m_defaultPalettes["qml/fs#VisualFileSelector"] = "VisualFileSelectorPropertiesPalette";
    m_defaultPalettes["qml/lcvimgproc#Blend"] = "BlendPalette";
    m_defaultPalettes["qml/lcvimgproc#Pad"] = "PadPalette";
    m_defaultPalettes["qml/lcvimgproc#TransformImage"] = "TransformPaletteProperties";
    m_defaultPalettes["qml/lcvcore#ImageRead"] = "ImageReadPalette";
    m_defaultPalettes["qml/lcvcore#ImageFile"] = "ImageFilePalette";
    m_defaultPalettes["qml/lcvcore#BlankImage"] = "BlankImagePalette";
    m_defaultPalettes["qml/lcvcore#DrawSurface"] = "DrawSurfacePalette";
    m_defaultPalettes["qml/lcvcore#ImageView"] = "ImageViewPalette";
    m_defaultPalettes["qml/lcvcore#VideoSurfaceView"] = "VideoSurfacePropertiesPalette";
    m_defaultPalettes["qml/lcvphoto#HueSaturationLightness"] = "HueSaturationLightnessPalette";
    m_defaultPalettes["qml/lcvphoto#Levels"] = "LevelsDefaultPalette";
    m_defaultPalettes["qml/lcvphoto#BrightnessAndContrast"] = "BrightnessAndContrastPalette";
    m_defaultPalettes["qml/lcvcore#PerspectiveOnBackground"] = "PerspectiveOnBackgroundPropertiesPalette";
    m_defaultPalettes["qml/lcvcore#ImageSegmentFactory"] = "ImageSegmentFactoryPropertiesPalette";
    m_defaultPalettes["qml/lcvcore#GalleryFileSelector"] = "GalleryFileSelectorPropertiesPalette";
    m_defaultPalettes["qml/lcvimgproc#ProjectionMapper"] = "ProjectionMapperPalette";


    if (parent){
        MLNode s = parent->readFor("qmljs");
        if ( s.type() == MLNode::Object )
            fromJson(s);
        else
            parent->write("qmljs", toJson());
    }
}

QmlJsSettings::~QmlJsSettings(){
}

void QmlJsSettings::fromJson(const MLNode &obj){
    if ( obj.hasKey("style") ){
        MLNode style = obj["style"];
        for ( auto it = style.begin(); it != style.end(); ++it ){
            QTextCharFormat& fmt = (*this)[QString::fromStdString(it.key())];
            if ( it.value().type() == MLNode::Object ){
                fmt.setBackground(QBrush(QColor(QString::fromStdString(it.value()["background"].asString()))));
                fmt.setForeground(QBrush(QColor(QString::fromStdString(it.value()["foreground"].asString()))));
            } else {
                fmt.clearBackground();
                fmt.setForeground(QBrush(QColor(QString::fromStdString(it.value().asString()))));
            }
        }
    }
    if ( obj.hasKey("palettes") ){
        MLNode palettes = obj["palettes"];
        if( palettes.hasKey("defaults") ){
            MLNode defaultPalettes = palettes["defaults"];
            const MLNode::ObjectType& ob = defaultPalettes.asObject();
            for ( auto it = ob.begin(); it != ob.end(); ++it ){
                m_defaultPalettes[QString::fromStdString(it->first)] = QString::fromStdString(it->second.asString());
            }
        }
    }
}

MLNode QmlJsSettings::toJson() const{
    MLNode root(MLNode::Object);
    MLNode style(MLNode::Object);

    for ( auto it = QmlJsSettings::rolesBegin(); it != QmlJsSettings::rolesEnd(); ++it )
    {
        const QTextCharFormat& fmt = (*this)[it.value()];

        if ( fmt.background().style() != Qt::NoBrush ){
            MLNode highlightob(MLNode::Object);
            highlightob["foreground"] = fmt.foreground().color().name().toStdString();
            highlightob["background"] = fmt.background().color().name().toStdString();
            style[it.key().toStdString()] = highlightob;
        } else {
            style[it.key().toStdString()] = fmt.foreground().color().name().toStdString();
        }
    }

    root["style"] = style;

    MLNode palettes(MLNode::Object);
    MLNode defaultPalettes(MLNode::Object);

    for ( auto it = m_defaultPalettes.begin(); it != m_defaultPalettes.end(); ++it ){
        defaultPalettes[it.key().toStdString()] = it.value().toStdString();
    }

    palettes["default"] = defaultPalettes;
    root["palettes"] = palettes;

    return root;
}

QString QmlJsSettings::defaultPalette(const QString &typeName) const{
    auto it = m_defaultPalettes.find(typeName);
    if ( it != m_defaultPalettes.end() ){
        return it.value();
    }
    return "";
}

void QmlJsSettings::__refresh(){
    EditorSettings* settings = qobject_cast<EditorSettings*>(parent());
    if ( !settings )
        return;

    MLNode s = settings->readFor("qmljs");
    if ( s.type() == MLNode::Object )
        fromJson(s);
}

QHash<QString, QmlJsSettings::ColorComponent> QmlJsSettings::m_formatRoles = QmlJsSettings::createFormatRoles();

QHash<QString, QmlJsSettings::ColorComponent> QmlJsSettings::createFormatRoles(){
    QHash<QString, QmlJsSettings::ColorComponent> formatRoles;
    formatRoles["text"]        = QmlJsSettings::Text;
    formatRoles["comment"]     = QmlJsSettings::Comment;
    formatRoles["number"]      = QmlJsSettings::Number;
    formatRoles["string"]      = QmlJsSettings::String;
    formatRoles["operator"]    = QmlJsSettings::Operator;
    formatRoles["identifier"]  = QmlJsSettings::Identifier;
    formatRoles["keyword"]     = QmlJsSettings::Keyword;
    formatRoles["builtin"]     = QmlJsSettings::BuiltIn;
    formatRoles["qmlproperty"] = QmlJsSettings::QmlProperty;
    formatRoles["qmltype"]     = QmlJsSettings::QmlType;
    formatRoles["qmlruntimeboundproperty"] = QmlJsSettings::QmlRuntimeBoundProperty;
    formatRoles["qmlruntimemodifiedvalue"] = QmlJsSettings::QmlRuntimeModifiedValue;
    formatRoles["qmledit"] = QmlJsSettings::QmlEdit;
    return formatRoles;
}


}// namespace
