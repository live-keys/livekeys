#ifndef QMLWATCHERBACKGROUND_H
#define QMLWATCHERBACKGROUND_H

#include <QThread>

namespace lv{

class QmlWatcherBackground : public QThread{

    Q_OBJECT

public:
    explicit QmlWatcherBackground(
        const QString& filePath,
        int lineNumber,
        const QString& source,
        QObject *parent = nullptr
    );
    ~QmlWatcherBackground() override;

    void run() override;

    const QString& result() const;

    bool existsInPath(const QString& path, const QString& name);

private:
    QString m_filePath;
    int     m_lineNumber;
    QString m_source;
    QString m_result;

};

inline const QString &QmlWatcherBackground::result() const{
    return m_result;
}

}// namespace

#endif // QMLWATCHERBACKGROUND_H
