#include "linecapture.h"
#include "live/visuallogqt.h"

namespace lv{

/**
 * \brief Collects messages off of a stream.
 *
 * Message format is:
 *
 * \code
 * Type:1;Len:12\0<contents>
 * \endcode
 */
LineCapture::LineCapture()
    : m_handlerData(0)
    , m_expectSize(0)
{
}

LineCapture::~LineCapture(){
}

void LineCapture::append(const QByteArray &data){
    if ( data.isEmpty() )
        return;

    if ( m_expectSize ){

        QByteArray left;

        if ( (unsigned long long)data.size() > m_expectSize ){
            left = data.mid(m_expectSize);

            m_current.data.append(data.mid(0, m_expectSize));
            m_expectSize = 0;

        } else {
            m_current.data.append(data);
            m_expectSize -= data.size();
        }

        if ( m_expectSize == 0 ){
            if ( m_handler )
                m_handler(m_current, m_handlerData);
            m_current = LineMessage();
        }

        if ( left.size() )
            append(left);

    } else {

        int separatorPos = -1;

        if ( m_current.data.size() > 0 && data.size() > 0 &&
             m_current.data[m_current.data.size() - 1] == '\r' &&
             data[0] == '\n' )
        {
            m_current.data = m_current.data.mid(0, m_current.data.size() - 1);
            separatorPos = 1;
        } else {
            for ( int i = 0; i < data.size(); ++i ){
                if ( data[i] == '\r' ){
                    if ( i < data.size() - 1 && data[i + 1] == '\n' ){
                        m_current.data.append(data.mid(0, i));
                        separatorPos = i + 2;
                        break;
                    }
                }
            }
        }

        if ( separatorPos > -1 ){

            int pos = m_current.data.lastIndexOf("Type:");
            if ( pos == -1 ){
                m_errorHandler(LineCapture::ParseTypeError, "Failed to find message type.");
                m_current.data.clear();
                append(data.mid(separatorPos));
            } else {
                if ( pos != 0 )
                    m_errorHandler(LineCapture::ParseLossData, "Data found before message type.");

                int typeNumberPos = pos + 5;
                int typeNumberPosEnd = m_current.data.indexOf(";Len:", typeNumberPos);

                if ( typeNumberPosEnd == -1 ){
                    m_errorHandler(LineCapture::ParseLengthError, "Failed to find message length.");
                    m_current.data.clear();
                    append(data.mid(separatorPos));
                } else {
                    int lenNumberPos = typeNumberPosEnd + 5;

                    bool parseTypeNumber;
                    bool parseLenNumber;

                    int type = m_current.data.mid(typeNumberPos, typeNumberPosEnd - typeNumberPos).toInt(&parseTypeNumber);
                    unsigned long long len = m_current.data.mid(lenNumberPos, separatorPos - lenNumberPos).toInt(&parseLenNumber);

                    if ( !parseTypeNumber ){
                        m_errorHandler(LineCapture::TypeNumberError, "Failed to parse message type as a number.");
                        m_current.data.clear();
                    } else if ( !parseLenNumber ){
                        m_errorHandler(LineCapture::LengthNumberError, "Failed to parse message length as a number.");
                        m_current.data.clear();
                    } else {
                        m_current.type = type;
                        m_current.data.clear();
                        m_expectSize = len;
                    }
                    append(data.mid(separatorPos));
                }
            }
        } else {
            m_current.data.append(data);
        }
    }
}



}// namespace
