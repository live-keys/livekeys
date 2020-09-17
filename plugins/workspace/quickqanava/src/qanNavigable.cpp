/*
 Copyright (c) 2008-2018, Benoit AUTHEMAN All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the author or Destrat.io nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL AUTHOR BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//-----------------------------------------------------------------------------
// This file is a part of the QuickQanava software library.
//
// \file	qanNavigable.cpp
// \author	benoit@destrat.io
// \date	2015 07 19
//-----------------------------------------------------------------------------

// Qt headers
// Nil

// QuickQanava headers
#include "./qanNavigable.h"

namespace qan { // ::qan

/* Navigable Object Management *///--------------------------------------------
Navigable::Navigable( QQuickItem* parent ) :
    QQuickItem{parent}
{
    _containerItem = new QQuickItem{this};
    _containerItem->setTransformOrigin( TransformOrigin::TopLeft );
    connect( _containerItem, &QQuickItem::childrenRectChanged,  // Listenning to children rect changes to update containerItem size.
             [this]() {
        if ( this->_containerItem ) {
            const auto cr = this->_containerItem->childrenRect();
            this->_containerItem->setWidth(cr.width());
            this->_containerItem->setHeight(cr.height());
        }
    });
    setAcceptedMouseButtons( Qt::RightButton | Qt::LeftButton );
    setTransformOrigin( TransformOrigin::TopLeft );

    _defaultGrid = std::make_unique<qan::Grid>();
    setGrid(_defaultGrid.get());
}
//-----------------------------------------------------------------------------

/* Navigation Management *///--------------------------------------------------
void    Navigable::setNavigable( bool navigable ) noexcept
{
    if ( navigable != _navigable ) {
        _navigable = navigable;
        emit navigableChanged();
    }
}

void    Navigable::centerOn( QQuickItem* item )
{
    // Algorithm:
        // 1. Project navigable view center in container item CS.
        // 2. Compute vector from navigable view center to item center in container item CS.
        // 3. Translate container by (-) this vector.
    if ( _containerItem == nullptr ||
         item == nullptr )
        return;
    if ( item->parentItem() != getContainerItem() )
        return;
    QPointF navigableCenter{ width() / 2., height() / 2. };
    QPointF navigableCenterContainerCs = mapToItem( _containerItem, navigableCenter );
    QPointF itemCenterContainerCs{ item->mapToItem( _containerItem, QPointF{ item->width() / 2., item->height() / 2. } ) };
    QPointF translation{ navigableCenterContainerCs - itemCenterContainerCs };
    _containerItem->setPosition( QPointF{ _containerItem->x() + translation.x(),
                                          _containerItem->y() + translation.y() } );
    updateGrid();
}

void    Navigable::fitInView( )
{
    QRectF content = _containerItem->childrenRect();
    if ( !content.isEmpty() ) { // Protect against div/0, can't fit if there is no content...
        qreal viewWidth = width();
        qreal viewHeight = height();

        qreal fitWidthZoom = 1.0;
        qreal fitHeightZoom = 1.0;
        fitWidthZoom = viewWidth / content.width();
        fitHeightZoom = viewHeight / content.height( );

        qreal fitZoom = fitWidthZoom;
        if ( content.height() * fitWidthZoom > viewHeight )
            fitZoom = fitHeightZoom;

        QPointF contentPos{0., 0.};
        if ( content.width() * fitZoom < viewWidth ) {  // Center zoomed content horizontally
            contentPos.rx() = ( viewWidth - ( content.width() * fitZoom ) ) / 2.;
        }
        if ( content.height() * fitZoom < viewHeight ) {   // Center zoomed content horizontally
            contentPos.ry() = ( viewHeight - ( content.height() * fitZoom ) ) / 2.;
        }
        _containerItem->setPosition( contentPos );
        _panModified = false;
        _zoomModified = false;

        // Don't use setZoom() because we force a TopLeft scale
        if ( isValidZoom(fitZoom) ) {
            _zoom = fitZoom;
            _containerItem->setScale(_zoom);
            emit zoomChanged();
            emit containerItemModified();
            navigableContainerItemModified();
            updateGrid();
        }
    }
}

void    Navigable::setAutoFitMode( AutoFitMode autoFitMode )
{
    if ( _autoFitMode != AutoFit &&
         autoFitMode == AutoFit )
        fitInView();    // When going to auto fit mode from another mode, force fitInView()

    _autoFitMode = autoFitMode;
    emit autoFitModeChanged();
}

void    Navigable::setZoom( qreal zoom )
{
    if ( isValidZoom( zoom ) ) {
        switch ( _zoomOrigin ) {
        case QQuickItem::Center: {
            zoomOn( QPointF{ width( ) / 2., height() / 2. },
                    zoom );
        }
            break;
        case QQuickItem::TopLeft:
        default:
            _zoom = zoom;
            _containerItem->setScale( _zoom );
            _zoomModified = true;
            emit zoomChanged();
            emit containerItemModified();
            navigableContainerItemModified();
        }
    }
}

void    Navigable::zoomOn( QPointF center, qreal zoom )
{
    // Get center coordinates in container CS, it is our
    // zoom application point
    qreal containerCenterX = center.x() - _containerItem->x();
    qreal containerCenterY = center.y() - _containerItem->y();
    qreal lastZoom = _zoom;

    // Don't apply modification if new zoom is not valid (probably because it is not in zoomMin, zoomMax range)
    if ( isValidZoom( zoom ) ) {
        // Get center coordinate in container CS with the new zoom
        qreal oldZoomX = containerCenterX * ( zoom / lastZoom );
        qreal oldZoomY = containerCenterY * ( zoom / lastZoom );

        // Compute a container position correction to have a fixed "zoom
        // application point"
        qreal zoomCorrectionX = containerCenterX - oldZoomX;
        qreal zoomCorrectionY = containerCenterY - oldZoomY;

        // Correct container position and set the appropriate scaling
        _containerItem->setX( _containerItem->x() + zoomCorrectionX );
        _containerItem->setY( _containerItem->y() + zoomCorrectionY );
        _containerItem->setScale( zoom );
        _zoom = zoom;
        _zoomModified = true;
        _panModified = true;
        emit zoomChanged();
        emit containerItemModified();
        navigableContainerItemModified();
        updateGrid();
    }
}

bool    Navigable::isValidZoom( qreal zoom ) const
{
    if ( qFuzzyCompare( 1. + zoom - _zoom, 1.0 ) )
        return false;
    if ( ( zoom > _zoomMin ) &&    // Don't zoom less than zoomMin
         ( _zoomMax < 0. ||   // Don't zoom more than zoomMax except if zoomMax is infinite
           zoom < _zoomMax ) )
            return true;
    return false;
}

void    Navigable::setZoomOrigin( QQuickItem::TransformOrigin zoomOrigin )
{
    if ( zoomOrigin != _zoomOrigin ) {
        _zoomOrigin = zoomOrigin;
        emit zoomOriginChanged();
    }
}

void    Navigable::setZoomMax( qreal zoomMax )
{
    if ( qFuzzyCompare( 1. + zoomMax - _zoomMax, 1.0 ) )
        return;
    _zoomMax = zoomMax;
    emit zoomMaxChanged();
}

void    Navigable::setZoomMin( qreal zoomMin )
{
    if ( qFuzzyCompare( 1. + zoomMin - _zoomMin, 1.0 ) )
        return;
    if ( zoomMin < 0.01 )
        return;
    _zoomMin = zoomMin;
    emit zoomMinChanged();
}

void    Navigable::setDragActive( bool dragActive ) noexcept
{
    if ( dragActive != _dragActive ) {
        _dragActive = dragActive;
        emit dragActiveChanged();
    }
}

void    Navigable::geometryChanged( const QRectF& newGeometry, const QRectF& oldGeometry )
{
    if (getNavigable()) {
        // Apply fitInView if auto fitting is set to true and the user has not applyed a custom zoom or pan
        if ( _autoFitMode == AutoFit &&
             ( !_panModified || !_zoomModified ) ) {
            fitInView();
        }
        // In AutoFit mode, try centering the content when the view is resized and the content
        // size is less than the view size (it is no fitting but centering...)
        if ( _autoFitMode == AutoFit ) {
            bool centerWidth = false;
            bool centerHeight = false;
            // Container item children Br mapped in root CS.
            QRectF contentBr = mapRectFromItem( _containerItem, _containerItem->childrenRect( ) );
            if ( newGeometry.contains( contentBr ) ) {
                centerWidth = true;
                centerHeight = true;
            } else {
                if ( contentBr.top() > newGeometry.top() &&
                     contentBr.bottom() < newGeometry.bottom() )
                    centerHeight = true;
                if ( contentBr.left() > newGeometry.left() &&
                     contentBr.right() < newGeometry.right() )
                    centerWidth = true;
            }
            if (centerWidth) {
                qreal cx = ( newGeometry.width() - contentBr.width() ) / 2.;
                _containerItem->setX( cx );
            }
            if (centerHeight) {
                qreal cy = ( newGeometry.height() - contentBr.height() ) / 2.;
                _containerItem->setY( cy );
            }
        }

        // In AutoFit mode, try anchoring the content to the visible border when the content has
        // custom user zoom
        if ( _autoFitMode == AutoFit &&
             ( _panModified || _zoomModified ) ) {
            bool anchorRight = false;
            bool anchorLeft = false;

            // Container item children Br mapped in root CS.
            QRectF contentBr = mapRectFromItem( _containerItem, _containerItem->childrenRect() );
            if ( contentBr.width() > newGeometry.width() &&
                 contentBr.right() < newGeometry.right() ) {
                anchorRight = true;
            }
            if ( contentBr.width() > newGeometry.width() &&
                 contentBr.left() > newGeometry.left() ) {
                anchorLeft = true;
            }
            if ( anchorRight ) {
                qreal xd = newGeometry.right() - contentBr.right();
                _containerItem->setX( _containerItem->x() + xd );
            } else if ( anchorLeft ) {  // Right anchoring has priority over left anchoring...
                qreal xd = newGeometry.left( ) - contentBr.left( );
                _containerItem->setX( _containerItem->x() + xd );
            }
        }

        updateGrid();
    }
    QQuickItem::geometryChanged( newGeometry, oldGeometry );
}

void    Navigable::mouseMoveEvent( QMouseEvent* event )
{
    if ( getNavigable() ) {
        if ( _leftButtonPressed && !_lastPan.isNull() ) {
            QPointF delta = _lastPan - event->localPos();
            QPointF p{ QPointF{ _containerItem->x(), _containerItem->y() } - delta };
            _containerItem->setX( p.x() );
            _containerItem->setY( p.y() );
            emit containerItemModified();
            navigableContainerItemModified();
            _panModified = true;
            _lastPan = event->localPos();
            setDragActive(true);

            updateGrid();
        }
    }
    QQuickItem::mouseMoveEvent( event );
}

void    Navigable::mousePressEvent( QMouseEvent* event )
{
    if ( getNavigable() ) {
        if ( event->button() == Qt::LeftButton ) {
            _leftButtonPressed = true;
            _lastPan = event->localPos();
            event->accept();
            return;
        }
        if ( event->button() == Qt::RightButton ) {
            event->accept();
            return;
        }
    }
    event->ignore();
}

void    Navigable::mouseReleaseEvent( QMouseEvent* event )
{
    if (getNavigable()) {
        if ( event->button() == Qt::LeftButton &&
             !_dragActive ) {       // Do not emit clicked when dragging occurs
            emit clicked( event->localPos() );
            navigableClicked( event->localPos() );
        } else if ( event->button() == Qt::RightButton ) {
            emit rightClicked( event->localPos() );
            navigableRightClicked(event->localPos() );
        }
        setDragActive(false);
        _leftButtonPressed = false;
    }
    QQuickItem::mouseReleaseEvent( event );
}

void Navigable::mouseDoubleClickEvent(QMouseEvent *event){
    if (getNavigable() && !_dragActive ){
        emit doubleClicked(event->localPos());
    }
    QQuickItem::mouseDoubleClickEvent(event);
}

void    Navigable::wheelEvent( QWheelEvent* event )
{
    if ( getNavigable() ) {
        qreal zoomFactor = ( event->angleDelta().y() > 0. ? _zoomIncrement : -_zoomIncrement );
        zoomOn( QPointF{ static_cast<qreal>(event->x()),
                         static_cast<qreal>(event->y()) },
                getZoom() + zoomFactor );
    } else {
        event->setAccepted(false);
        return;
    }
    updateGrid();

    // Note 20160117: NavigableArea is opaque for wheel events
    //QQuickItem::wheelEvent( event );
}
//-----------------------------------------------------------------------------

/* Grid Management *///--------------------------------------------------------
void    Navigable::setGrid( qan::Grid* grid ) noexcept
{
    if ( grid != _grid ) {
        if ( _grid ) {                    // Hide previous grid
            disconnect( _grid, nullptr,
                        this, nullptr );  // Disconnect every update signals from grid to this navigable
        }

        _grid = grid;                       // Configure new grid
        if ( _grid ) {
            _grid->setParentItem( this );
            _grid->setZ( -1.0 );
            _grid->setAntialiasing(false);
            _grid->setScale(1.0);
            connect(grid,   &QQuickItem::visibleChanged, // Force updateGrid when visibility is changed to eventually
                    this,   &Navigable::updateGrid);    // take into account any grid property change while grid was hidden.
        }
        if (!_grid)
            _grid = _defaultGrid.get(); // Do not connect default grid, it is an "empty grid"
        updateGrid();
        emit gridChanged();
    }
}

void    Navigable::updateGrid() noexcept
{
    if ( _grid &&
         _containerItem != nullptr ) {
        // Generate a view rect to update grid
        QRectF viewRect{ _containerItem->mapFromItem(this, {0.,0.}),
                         _containerItem->mapFromItem(this, {width(), height()}) };
        if (!viewRect.isEmpty())
            _grid->updateGrid(viewRect, *_containerItem, *this );
    }
}
//-----------------------------------------------------------------------------

} // ::qan
