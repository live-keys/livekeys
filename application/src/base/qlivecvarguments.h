#ifndef QLIVECVARGUMENTS_HPP
#define QLIVECVARGUMENTS_HPP

#include <QObject>

class QLiveCVCommandLineParser;
class QLiveCVArguments : public QObject{

    Q_OBJECT
    Q_PROPERTY(bool previewFlag READ previewFlag)

public:
    explicit QLiveCVArguments(
        const QString& header,
        int argc,
        const char* const argv[],
        QObject *parent = 0
    );
    ~QLiveCVArguments();

    bool previewFlag() const;
    bool consoleFlag() const;
    bool fileLogFlag() const;

    const QString& script() const;

private:
    void initialize(int argc, const char* const argv[]);

    QLiveCVCommandLineParser* m_parser;

    bool m_fileLogFlag;
    bool m_consoleFlag;
    bool m_previewFlag;

    QString m_script;

};

inline bool QLiveCVArguments::previewFlag() const{
    return m_previewFlag;
}

inline bool QLiveCVArguments::consoleFlag() const{
    return m_consoleFlag;
}

inline bool QLiveCVArguments::fileLogFlag() const{
    return m_fileLogFlag;
}

inline const QString&QLiveCVArguments::script() const{
    return m_script;
}

#endif // QLIVECVARGUMENTS_HPP
