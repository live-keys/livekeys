/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef QKEYPOINTTOSCENEMAP_H
#define QKEYPOINTTOSCENEMAP_H

#include <QObject>
#include "qmat.h"
#include "qlcvfeatures2dglobal.h"

class Q_LCVFEATURES2D_EXPORT QKeyPointToSceneMap : public QObject{

    Q_OBJECT

public:
    class ObjectKeypointToScene{
    public:
        std::vector<cv::Point2f> objectPoints;
        std::vector<cv::Point2f> scenePoints;
    };

    typedef std::vector<ObjectKeypointToScene*> Container;
    typedef Container::iterator       Iterator;
    typedef Container::const_iterator ConstIterator;

public:
    QKeyPointToSceneMap(QObject* parent = 0);
    ~QKeyPointToSceneMap();

    void append(ObjectKeypointToScene* objkeypointToScene);
    ObjectKeypointToScene* mappingAt(size_t index);

    void resize(size_t size);

    Iterator begin();
    Iterator end();
    ConstIterator begin() const;
    ConstIterator end() const;

public slots:
    int size() const;

private:
    std::vector<ObjectKeypointToScene*> m_mappings;

};

inline void QKeyPointToSceneMap::append(QKeyPointToSceneMap::ObjectKeypointToScene *objkeypointToScene){
    m_mappings.push_back(objkeypointToScene);
}

inline QKeyPointToSceneMap::ObjectKeypointToScene *QKeyPointToSceneMap::mappingAt(size_t index){
    return m_mappings[index];
}

inline void QKeyPointToSceneMap::resize(size_t size){
    if ( size < m_mappings.size() ){
        for ( size_t i = size; i < m_mappings.size(); ++i ){
            delete m_mappings[i];
        }
    }
    m_mappings.resize(size, 0);
    for ( size_t i = 0; i < m_mappings.size(); ++i ){
        if ( m_mappings[i] )
            delete m_mappings[i];
        m_mappings[i] = new QKeyPointToSceneMap::ObjectKeypointToScene;
    }
}

inline QKeyPointToSceneMap::Iterator QKeyPointToSceneMap::begin(){
    return m_mappings.begin();
}

inline QKeyPointToSceneMap::Iterator QKeyPointToSceneMap::end(){
    return m_mappings.end();
}

inline QKeyPointToSceneMap::ConstIterator QKeyPointToSceneMap::begin() const{
    return m_mappings.begin();
}

inline QKeyPointToSceneMap::ConstIterator QKeyPointToSceneMap::end() const{
    return m_mappings.end();
}

inline int QKeyPointToSceneMap::size() const{
    return (int)m_mappings.size();
}

#endif // QKEYPOINTTOSCENEMAP_H
