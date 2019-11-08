#ifndef LANGUAGELVHIGHLIGHTER_H
#define LANGUAGELVHIGHLIGHTER_H

#include <QTextDocument>

#include "editlvsettings.h"
#include "live/documenthandler.h"
#include "live/syntaxhighlighter.h"

namespace lv{

class LanguageLvHighlighter : public SyntaxHighlighter{

public:
    explicit LanguageLvHighlighter(EditLvSettings* settings, DocumentHandler* handler, QTextDocument *parent = nullptr);
    ~LanguageLvHighlighter() override;

    void setTarget(QTextDocument* target);

protected:
    QList<TextFormatRange> highlight(int lastUserState, int position, const QString& text) override;
    QList<TextFormatRange> highlightSections(const QList<ProjectDocumentSection::Ptr>&) override;

private:
    EditLvSettings*  m_settings;
};


inline void LanguageLvHighlighter::setTarget(QTextDocument *target){
    setDocument(target);
}

}// namespace

#endif // LANGUAGELVHIGHLIGHTER_H
