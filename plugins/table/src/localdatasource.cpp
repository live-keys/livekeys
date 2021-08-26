#include "localdatasource.h"
#include "live/viewengine.h"
#include "live/exception.h"

#include "csv.hpp"

#include <QtDebug>

#include <QFile>

namespace lv{

LocalDataSource::LocalDataSource(QObject *parent)
    : TableDataSource(parent)
{

}

LocalDataSource::~LocalDataSource(){
}

int LocalDataSource::totalRows() const{
    return m_data.size();
}

QJSValue LocalDataSource::rowAt(int index){
    ViewEngine* ve = ViewEngine::grab(this);
    if (!ve)
        return QJSValue();
    if (index >= m_data.length() )
        return QJSValue();

    auto& rowData = m_data[index];
    QJSValue result = ve->engine()->newArray(rowData.length());
    for ( int i = 0; i < rowData.length(); ++i ){
        result.setProperty(i, rowData[i]);
    }
    return result;
}

QJSValue LocalDataSource::columnInfo() const{
    ViewEngine* ve = ViewEngine::grab(this);
    if (!ve)
        return QJSValue();

    QJSValue result = ve->engine()->newArray(m_headers.length());
    for ( int i = 0; i < m_headers.length(); ++i ){
        result.setProperty(i, m_headers[i]);
    }
    return result;
}

void LocalDataSource::clear(){
    beginLoadData();
    m_data.clear();
    m_headers.clear();
    endLoadData();
}

QString LocalDataSource::valueAt(int row, int column){
    auto& rowData = m_data[row];
    return rowData[column];
}

void LocalDataSource::setValueAt(int row, int column, const QString &value){
    m_data[row][column] = value;
}

void LocalDataSource::addRow(){
    size_t s = m_headers.size();
    QList<QString> row;
    for ( size_t i = 0; i < s; ++i ){
        row.append("");
    }
    m_data.append(row);
}

void LocalDataSource::addColumn(){
    m_headers.append("");
    for ( auto it = m_data.begin(); it != m_data.end(); ++it ){
        (*it).append("");
    }
}

void LocalDataSource::removeColumn(int idx){

}

void LocalDataSource::removeRow(int idx){
    m_data.erase(m_data.begin() + idx);
}

int LocalDataSource::totalColumns() const{
    return m_headers.size();
}

QList<QString> LocalDataSource::columnNames() const{
    return m_headers;
}

void LocalDataSource::readFromFile(const QString &path, const QJSValue &options){
    beginLoadData();

    m_data.clear();
    m_headers.clear();

    csv::CSVFormat format;
    format.no_header();
    csv::CSVReader reader(path.toStdString(), format);

    bool firstRow = true;
    for (csv::CSVRow& row: reader) { // Input iterator
        QList<QString> dataRow;
        if (firstRow){
            bool useHeader = options.hasOwnProperty("header") && options.property("header").toBool();
            for (csv::CSVField& field: row) {
                if ( useHeader ){
                    QString headerName = QString::fromStdString(field.get<std::string>());
                    m_headers.append(headerName);
                } else {
                    m_headers.append("");
                    dataRow.append(QString::fromStdString(field.get<std::string>()));
                }
            }
            if( !useHeader )
                m_data.append(dataRow);
            firstRow = false;
        } else {
            for (csv::CSVField& field: row) {
                dataRow.append(QString::fromStdString(field.get<std::string>()));
            }
            m_data.append(dataRow);
        }
    }

    endLoadData();
}

void LocalDataSource::writeToFile(const QString &path){
    QFile file(path);
    if ( !file.open(QFile::WriteOnly) ){
        ViewEngine* engine = ViewEngine::grab(this);
        Exception e = CREATE_EXCEPTION(lv::Exception, "Failed to write to file: " + path.toStdString() + ".", Exception::toCode("~FileWrite"));
        engine->throwError(&e, this);
        return;
    }

    QTextStream stream(&file);

    for ( auto it = m_data.begin(); it != m_data.end(); ++it ){
        const QList<QString>& row = *it;
        for ( auto rt = row.begin(); rt != row.end(); ++rt ){
            if ( rt != row.begin() )
                stream << ",";

            const QString& cellStr = *rt;

            stream << "\"";

            int from = 0;
            int index = cellStr.indexOf('"');
            while ( index != -1 ){
                stream << cellStr.mid(from, index - from) << "\\\"";
                from = index + 1;
                index = cellStr.indexOf('"', from);
            }

            if ( from != 0 ){
                stream << cellStr.mid(from);
            } else {
                stream << cellStr;
            }

            stream << "\"";

        }
        stream << "\n";
    }
}

}// namespace
