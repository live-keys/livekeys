#ifndef LVEDITLVSETTINGS_H
#define LVEDITLVSETTINGS_H

#include <QObject>
#include <QHash>
#include <QTextCharFormat>

#include "live/mlnode.h"
#include "live/editorsettings.h"

namespace lv{

/// \private
class EditLvSettings : public QObject{

    Q_OBJECT

public:
    EditLvSettings(EditorSettings* parent = nullptr);
    virtual ~EditLvSettings() override;

    void fromJson(const MLNode &json);
    MLNode toJson() const;

    bool hasKey(const std::string &key);

    QTextCharFormat& operator[](const std::string& key);
    QTextCharFormat operator[](const std::string& key) const;

public slots:
    void __refresh();

private:
    QTextCharFormat createFormat(const QColor& foreground);
    QTextCharFormat createFormat(const QColor& foreground, const QColor& background);

    QMap<std::string, QTextCharFormat> m_formats;
};

inline bool EditLvSettings::hasKey(const std::string &key){
    return m_formats.contains(key);
}

inline QTextCharFormat &EditLvSettings::operator[](const std::string &key){
    return m_formats[key];
}

inline QTextCharFormat EditLvSettings::operator[](const std::string &key) const{
    return m_formats[key];
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
