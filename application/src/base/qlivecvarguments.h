#ifndef QLIVECVARGUMENTS_HPP
#define QLIVECVARGUMENTS_HPP

#include <QObject>

namespace lcv{

class QLiveCVCommandLineParser;
class QLiveCVArguments : public QObject{

    Q_OBJECT
    Q_PROPERTY(bool previewFlag READ previewFlag CONSTANT)

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

    bool helpFlag() const;
    bool versionFlag() const;
    QString helpString() const;

    const QStringList& monitoredFiles() const;

    QLiveCVCommandLineParser* parser();

public slots:
    const QString& script() const;
    QString at(int number) const;
    int length() const;
    QString option(const QString& key) const;
    bool isOptionSet(const QString& key) const;

private:
    void initialize(int argc, const char* const argv[]);

    QLiveCVCommandLineParser* m_parser;

    bool m_fileLogFlag;
    bool m_consoleFlag;
    bool m_previewFlag;

    QStringList m_monitoredFiles;

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

inline const QStringList &QLiveCVArguments::monitoredFiles() const{
    return m_monitoredFiles;
}

inline QLiveCVCommandLineParser *QLiveCVArguments::parser(){
    return m_parser;
}

inline const QString&QLiveCVArguments::script() const{
    return m_script;
}

}// namespace

#endif // QLIVECVARGUMENTS_HPP
