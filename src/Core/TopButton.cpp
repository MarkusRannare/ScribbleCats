#include "TopButton.h"
#include <cstring>

#include <Core/memory.h>
#include "GUIHandler.h"
#include "array_functions.h"
#include <hge/hgesprite.h>

using namespace foundation;

#define PADDING 5
#define ITEM_HEIGHT ( mHandler->GetFont()->GetHeight() * fSettings.Scale + 2 * PADDING )

namespace Scribble
{
	static FontSettings fSettings = { 0.5f, ARGB( 255, 0, 0, 0 ) };
	
	TopButton::TopButton( const char* Text, GUIHandler* Handler ) :
		mText( (char*)memory_globals::default_allocator().allocate( strlen( Text ) + 1 ) ),
		mHandler( Handler ),
		mExpanded( false ),
		mSubTexts( memory_globals::default_allocator() ),
		mSprite( MAKE_NEW( memory_globals::default_allocator(), hgeSprite, NULL, 0, 0, 100, 100 ) )
	{
		memcpy_s( mText, strlen( Text ) + 1, Text, strlen( Text ) + 1 );
		mHandler->SetFontSettings( fSettings );
		mWidth = mHandler->GetFont()->GetStringWidth( mText );
		mExpandedHeight = ITEM_HEIGHT;
		mSprite->SetColor( ARGB( 255, 200, 200, 200 ) );
	}

	TopButton::~TopButton()
	{
		memory_globals::default_allocator().deallocate( mText );

		for( uint32_t Idx = 0; Idx < array::size( mSubTexts ); ++Idx )
		{
			memory_globals::default_allocator().deallocate( mSubTexts[Idx] );
		}

		MAKE_DELETE( memory_globals::default_allocator(), hgeSprite, mSprite );
	}

	void TopButton::AddSubText( const char* Text )
	{
		char* TextContent = (char*)memory_globals::default_allocator().allocate( strlen( Text ) + 1 );
		memcpy_s( TextContent, strlen( Text ) + 1, Text, strlen( Text ) + 1 );

		array::push_back( mSubTexts, TextContent );

		mHandler->SetFontSettings( fSettings );
		float LargestWidth = mHandler->GetFont()->GetStringWidth( mText );
		for( uint32_t Idx = 0; Idx < array::size( mSubTexts ); ++Idx )
		{
			char* SubText = mSubTexts[Idx];
			float TempWidth = mHandler->GetFont()->GetStringWidth( SubText );
			if( TempWidth > LargestWidth )
			{
				LargestWidth = TempWidth;
			}
		}

		mWidth = LargestWidth;
		mExpandedHeight = ( array::size( mSubTexts ) + 1 ) * ITEM_HEIGHT;
	}

	void TopButton::Update( float DeltaTime )
	{
	}

	void TopButton::Render()
	{
		mHandler->SetFontSettings( fSettings );
		if( mSelectedIndex == -1 && mExpanded )
		{
			FontSettings Settings = fSettings;
			Settings.Color = ARGB( 255, 255, 0, 0 );
			mHandler->SetFontSettings( Settings );
		}
		mHandler->Printf( mLocation.X, mLocation.Y, HGETEXT_LEFT, "%s", mText );
		if( mExpanded )
		{
			float YLocation = ITEM_HEIGHT;
			mSprite->RenderStretch( mLocation.X, YLocation, mLocation.X + mWidth, YLocation + mExpandedHeight - ITEM_HEIGHT );
			YLocation += PADDING;
			for( uint32_t Idx = 0; Idx < array::size( mSubTexts ); ++Idx )
			{
				FontSettings Settings = fSettings;
				if( Idx == mSelectedIndex )
				{
					Settings.Color = ARGB( 255, 255, 0, 0 );
				}
				mHandler->SetFontSettings( Settings );
				mHandler->Printf( mLocation.X, YLocation, HGETEXT_LEFT, "%s", mSubTexts[Idx] );
				YLocation += ITEM_HEIGHT;
			}
		}
	}

	void TopButton::MouseOver( bool Over )
	{
		mExpanded = Over;
	}

	bool TopButton::MouseLButton( bool Down )
	{
		if( mExpanded )
		{
			mSelectedIndex = mSelectedIndex;
		}

		return false;
	}

	bool TopButton::MouseMove( float X, float Y )
	{
		mSelectedIndex = (int)floor( Y / ITEM_HEIGHT ) - 1;

		mMouseOffset.X = X;
		mMouseOffset.Y = Y;

		return false;
	}

	void TopButton::SetLocation( float X, float Y, float& Out_NextX )
	{
		mLocation.X = X;
		mLocation.Y = Y;

		Out_NextX = mLocation.X + mWidth + 10.0f;
		
		rect = hgeRect( mLocation.X, mLocation.Y, mLocation.X + mWidth, mLocation.Y + ( mExpanded ? mExpandedHeight : ITEM_HEIGHT ) );
	}
} 