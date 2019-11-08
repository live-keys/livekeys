#ifndef LVEDITLVSETTINGS_H
#define LVEDITLVSETTINGS_H

#include <QObject>
#include "live/editorsettingscategory.h"
#include <QHash>
#include <QTextCharFormat>

namespace lv{

/// \private
class EditLvSettings : public EditorSettingsCategory{

public:
    enum ColorComponent{
        Text,
        Comment,
        Number,
        String,
        Operator,
        Identifier,
        Keyword,
        BuiltIn,
        LvProperty,
        LvType,
        LvRuntimeBoundProperty,
        LvRuntimeModifiedValue,
        LvEdit
    };

public:
    EditLvSettings();
    virtual ~EditLvSettings() override;


    void fromJson(const QJsonValue &json) override;
    QJsonValue toJson() const override;

    bool hasKey(const QString& key);

    QTextCharFormat& operator[](const QString& key);
    QTextCharFormat& operator[](const ColorComponent& key);

    QTextCharFormat operator[](const QString& key) const;
    QTextCharFormat operator[](const ColorComponent& key) const;

    static QHash<QString, ColorComponent>::ConstIterator rolesBegin();
    static QHash<QString, ColorComponent>::ConstIterator rolesEnd();

private:
    QTextCharFormat createFormat(const QColor& foreground);
    QTextCharFormat createFormat(const QColor& foreground, const QColor& background);

    static QHash<QString, ColorComponent> createFormatRoles();
    static QHash<QString, ColorComponent> m_formatRoles;

    QHash<ColorComponent, QTextCharFormat> m_formats;
};

inline QTextCharFormat &EditLvSettings::operator[](const QString &key){
    return m_formats[m_formatRoles[key]];
}

inline QTextCharFormat &EditLvSettings::operator[](const EditLvSettings::ColorComponent &key){
    return m_formats[key];
}

inline QTextCharFormat EditLvSettings::operator[](const QString &key) const{
    return m_formats[m_formatRoles[key]];
}

inline QTextCharFormat EditLvSettings::operator[](const EditLvSettings::ColorComponent &key) const{
    return m_formats[key];
}

inline bool EditLvSettings::hasKey(const QString &key){
    return m_formatRoles.contains(key);
}

inline QHash<QString, EditLvSettings::ColorComponent>::ConstIterator EditLvSettings::rolesBegin(){
    return m_formatRoles.begin();
}

inline QHash<QString, EditLvSettings::ColorComponent>::ConstIterator EditLvSettings::rolesEnd(){
    return m_formatRoles.end();
}

inline QTextCharFormat EditLvSettings::createFormat(const QColor &foreground){
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(foreground));
    return fmt;
}

inline QTextCharFormat EditLvSettings::createFormat(const QColor &foreground, const QColor &background){
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(foreground));
    fmt.setBackground(QBrush(background));
    return fmt;
}

}// namespace

#endif // LVEDITLVSETTINGS_H
