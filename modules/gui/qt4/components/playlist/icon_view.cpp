/*****************************************************************************
 * icon_view.cpp : Icon view for the Playlist
 ****************************************************************************
 * Copyright © 2010 the VideoLAN team
 * $Id$
 *
 * Authors:         Jean-Baptiste Kempf <jb@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include "components/playlist/icon_view.hpp"
#include "components/playlist/playlist_model.hpp"
#include "input_manager.hpp"

#include <QApplication>
#include <QPainter>
#include <QRect>
#include <QStyleOptionViewItem>
#include <QFontMetrics>

#include "assert.h"

#define RECT_SIZE           100
#define ART_SIZE            64
#define OFFSET              (RECT_SIZE-64)/2
#define ITEMS_SPACING       10
#define ART_RADIUS          5

void PlListViewItemDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    painter->setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform );

    /*if( option.state & QStyle::State_Selected )
         painter->fillRect(option.rect, option.palette.highlight());*/
    QApplication::style()->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, painter );

    PLItem *currentItem = static_cast<PLItem*>( index.internalPointer() );
    assert( currentItem );

    QPixmap pix;
    QString url = InputManager::decodeArtURL( currentItem->inputItem() );

    if( !url.isEmpty() && pix.load( url ) )
    {
        pix = pix.scaled( ART_SIZE, ART_SIZE, Qt::KeepAspectRatioByExpanding );
    }
    else
    {
        pix = QPixmap( ":/noart64" );
    }

    QRect artRect = option.rect.adjusted( OFFSET - 1, 0, - OFFSET, - OFFSET *2 );
    QPainterPath artRectPath;
    artRectPath.addRoundedRect( artRect, ART_RADIUS, ART_RADIUS );

    // Draw the drop shadow
    painter->save();
    painter->setOpacity( 0.7 );
    painter->setBrush( QBrush( Qt::gray ) );
    painter->drawRoundedRect( artRect.adjusted( 2, 2, 2, 2 ), ART_RADIUS, ART_RADIUS );
    painter->restore();

    // Draw the art pixmap
    painter->setClipPath( artRectPath );
    painter->drawPixmap( artRect, pix );
    painter->setClipping( false );

    QColor text = qApp->palette().text().color();
    QString title = qfu( input_item_GetTitleFbName( currentItem->inputItem() ) );
    QString artist = qfu( input_item_GetArtist( currentItem->inputItem() ) );

    painter->setPen( text );
    painter->setFont( QFont( "Verdana", 7, QFont::Bold ) );
    QFontMetrics fm = painter->fontMetrics();
    QRect titleRect = option.rect.adjusted( 1, ART_SIZE + 4, 0, -1 );
    titleRect.setHeight( fm.height() + 2 );

    painter->drawText( titleRect, fm.elidedText( title, Qt::ElideRight, titleRect.width() ),
                       QTextOption( Qt::AlignCenter ) );

    painter->setPen( text.lighter( 240 ) );
    painter->setFont( QFont( "Verdana", 7 ) );
    fm = painter->fontMetrics();
    QRect artistRect = option.rect.adjusted( 1, ART_SIZE + 4 + titleRect.height(), -1, -1 );

    painter->drawText( artistRect, fm.elidedText( artist, Qt::ElideRight, artistRect.width() ),
                       QTextOption( Qt::AlignCenter ) );
}

QSize PlListViewItemDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    return QSize( RECT_SIZE, RECT_SIZE);
}


PlIconView::PlIconView( PLModel *model, QWidget *parent ) : QListView( parent )
{
    setModel( model );
    setViewMode( QListView::IconMode );
    setMovement( QListView::Static );
    setResizeMode( QListView::Adjust );
    setGridSize( QSize( RECT_SIZE, RECT_SIZE ) );
    setUniformItemSizes( true );
    setSpacing( ITEMS_SPACING );
    setWrapping( true );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    setAcceptDrops( true );

    PlListViewItemDelegate *pl = new PlListViewItemDelegate();
    setItemDelegate( pl );

    CONNECT( this, activated( const QModelIndex & ), this, activate( const QModelIndex & ) );
}

void PlIconView::activate( const QModelIndex & index )
{
    if( model()->hasChildren( index ) )
        setRootIndex( index );
    else
    {
        PLModel *plModel = qobject_cast<PLModel*>( model() );
        if( !plModel ) return;
        plModel->activateItem( index );
    }
}