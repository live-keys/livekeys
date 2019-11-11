#ifndef TEXTDOCUMENTDATA_H
#define TEXTDOCUMENTDATA_H

#include <vector>
typedef unsigned short int ushort;

class QTextDocument;

namespace lv {

class TextDocumentData
{
public:
    TextDocumentData();
    std::vector<std::pair<unsigned, unsigned>> contentsChange(QTextDocument* document, int position, int removed, int added);
    void clear();
    std::vector<ushort> &rowAt(unsigned i);
    unsigned size() { return rows.size(); }
private:
    std::vector<std::vector<ushort>> rows;
};

}

#endif // TEXTDOCUMENTDATA_H
