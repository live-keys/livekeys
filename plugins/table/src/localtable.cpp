#include "localtable.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "tableschema.h"
#include "tablemodel.h"

#include "csv.hpp"

#include <QtDebug>
#include <QFile>
#include <QJSValueIterator>

namespace lv{

LocalTable::LocalTable(QObject *parent)
    : Table(parent)
{
    setSchema(new TableSchema);
}

LocalTable::~LocalTable(){
}

void LocalTable::clear(){
    m_data.clear();
    setSchema(new TableSchema);
}

QString LocalTable::valueAt(int row, int column){
    auto& rowData = m_data[row];
    return rowData[column];
}

void LocalTable::insertRow(const QJSValue &row, Table::InsertCallback cb){
    if ( !schema() )
        return;

    int s = schema()->totalFields();
    QList<QString> rowToAdd;
    for ( int i = 0; i < s; ++i ){
        rowToAdd.append("");
    }

    if ( row.isArray() ){
        QJSValueIterator it(row);
        while ( it.hasNext() ){
            it.next();
            if ( it.name() != "length" ){
                bool toIntOk = false;
                int key = it.name().toInt(&toIntOk);
                if ( toIntOk ){
                    rowToAdd[key] = it.value().toString();
                }
            }
        }
    } else if ( row.isObject() ){
        QJSValueIterator it(row);
        while ( it.hasNext() ){
            it.next();
            bool toIntOk = false;
            int key = it.name().toInt(&toIntOk);
            if ( toIntOk ){
                rowToAdd[key] = it.value().toString();
            }
        }
    }

    m_data.append(rowToAdd);

    if (cb)
        cb(m_data.size() - 1);
}

void LocalTable::updateRow(int index, const QJSValue &row){
    if ( !schema() )
        return;
    if ( index >= totalRows() )
        return;

    int totalFields = schema()->totalFields();

    if ( row.isArray() ){
        QJSValueIterator it(row);
        while ( it.hasNext() ){
            it.next();
            if ( it.name() != "length" ){
                bool toIntOk = false;
                int key = it.name().toInt(&toIntOk);
                if ( toIntOk && key < totalFields ){
                    m_data[index][key] = it.value().toString();
                }
            }
        }
    } else if ( row.isObject() ){
        QJSValueIterator it(row);
        while ( it.hasNext() ){
            it.next();
            bool toIntOk = false;
            int key = it.name().toInt(&toIntOk);
            if ( toIntOk && key < totalFields ){
                m_data[index][key] = it.value().toString();
            }
        }
    }
}

void LocalTable::removeRow(int idx){
    m_data.erase(m_data.begin() + idx);
}

int LocalTable::totalRows() const{
    return m_data.size();
}

void LocalTable::insertField(int index, TableField::Ptr field){
    if ( !schema() )
        return;

    int totalFields = schema()->totalFields();
    schema()->insertField(index, field);
    if ( index < 0 || index > totalFields ){
        for ( auto it = m_data.begin(); it != m_data.end(); ++it ){
            (*it).append("");
        }
    } else {
        for ( auto it = m_data.begin(); it != m_data.end(); ++it ){
            (*it).insert(index, "");
        }
    }
}

void LocalTable::updateField(int index, const QJSValue &opt){

}

void LocalTable::removeField(int /*index*/){
    if ( !schema() )
        return;

    //TODO
}

QJSValue LocalTable::rowAt(int index){
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



void LocalTable::readFromFile(const QString &path, const QJSValue &options){
    if ( isModelSet() )
        model()->notifyTableRefresh();

    m_data.clear();
    setSchema(new TableSchema);

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
                    schema()->insertField(-1, TableField::create(headerName));
                } else {
                    schema()->insertField(-1, TableField::create(""));
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

    if ( isModelSet() )
        model()->notifyTableRefreshEnd();
}

void LocalTable::writeToFile(const QString &path, const QJSValue& options){
    if ( schema() )
        return;

    QFile file(path);
    if ( !file.open(QFile::WriteOnly) ){
        ViewEngine* engine = ViewEngine::grab(this);
        Exception e = CREATE_EXCEPTION(lv::Exception, "Failed to write to file: " + path.toStdString() + ".", Exception::toCode("~FileWrite"));
        engine->throwError(&e, this);
        return;
    }

    QTextStream stream(&file);

    bool useHeader = options.hasOwnProperty("header") && options.property("header").toBool();
    if ( useHeader && schema()->totalFields() ){
        stream << "\"" << schema()->fieldAt(0)->name() << "\"";
        for ( int i = 1; i < schema()->totalFields(); ++i ){
            stream << ",\"" << schema()->fieldAt(i)->name() << "\"";
        }
        stream << "\n";
    }

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
