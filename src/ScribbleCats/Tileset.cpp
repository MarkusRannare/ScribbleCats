#include "Tileset.h"
#include <foundation/memory.h>
#include "array_functions.h"
#include <hge/hgesprite.h>

using namespace foundation;

namespace Scribble
{
	Tileset::Tileset( float TileWidth, float OriginX, float OriginY ) :
		mLayers( memory_globals::default_allocator() ),
		mTileWidth( TileWidth ),
		mSprite( MAKE_NEW( memory_globals::default_allocator(), hgeSprite, NULL, 0, 0, TileWidth, TileWidth ) )
	{
		mSprite->SetBlendMode( BLEND_ALPHABLEND );
	}

	Tileset::~Tileset()
	{
		for( size_t I = 0; I < array::size( mLayers ); ++I )
		{
			memory_globals::default_allocator().deallocate( mLayers[I].TileData );
			memory_globals::default_allocator().deallocate( mLayers[I].CollisionData );
		}

		MAKE_DELETE( memory_globals::default_allocator(), hgeSprite, mSprite );
	}

	void Tileset::AddLayer( int NumTilesX, INT NumTilesY, short* TileData, HTEXTURE SourceTexture )
	{
		int Index = array::add_zeroed( mLayers );

		TileLayer& Layer = mLayers[Index];

		Layer.NumTilesX = NumTilesX;
		Layer.NumTilesY = NumTilesY;
		Layer.SourceTexture = SourceTexture;
		Layer.TileData = TileData;
		
		CalculateCollisionData( NumTilesX, NumTilesY, TileData, Layer );
	}

	void Tileset::CalculateCollisionData( int NumTilesX, int NumTilesY, short* TileData, TileLayer& Layer )
	{
		// If we already have collision data, Zero-it and start over
		if( Layer.CollisionData )
		{
			memory_globals::default_allocator().deallocate( Layer.CollisionData );
			Layer.NumCollisionEntries = 0;
		}

		// This should be changed as it might become quite big buffer if we increase the level size
		// it is already ~156.25kb on a 100*100 tileset
		hgeRect* TempCollisionData = (hgeRect*)memory_globals::default_scratch_allocator().allocate( NumTilesX * NumTilesY * sizeof(hgeRect) );
		for( int Y = 0; Y < NumTilesY; ++Y )
		{
			for( int X = 0; X < NumTilesX; ++X )
			{
				// 0 means no collision
				short TileId = GetTileId( TileData[X+Y*NumTilesX] );
				if( TileId >= 0 )
				{
					hgeRect& Collision = TempCollisionData[Layer.NumCollisionEntries++];
					Collision.Set( 
						X * mTileWidth,					// x1
						Y * mTileWidth,					// y1
						X * mTileWidth + mTileWidth,	// x2
						Y * mTileWidth + mTileWidth );	// y2
				}
			}
		}

		Layer.CollisionData = (hgeRect*)memory_globals::default_allocator().allocate( Layer.NumCollisionEntries * sizeof(hgeRect) );
		memcpy_s( Layer.CollisionData, Layer.NumCollisionEntries * sizeof(hgeRect), TempCollisionData, Layer.NumCollisionEntries * sizeof(hgeRect) );
		memory_globals::default_scratch_allocator().deallocate( TempCollisionData );
	}

	hgeRect* Tileset::GetCollisionForLayer( int Layer, int& out_NumCollisions )
	{
		out_NumCollisions = mLayers[Layer].NumCollisionEntries;

		return mLayers[Layer].CollisionData;
	}

	void Tileset::Tick( float CenterX, float CenterY )
	{
	}

	void Tileset::Render()
	{
		extern HGE* g_Hge;

		for( size_t I = 0; I < array::size( mLayers ); ++I )
		{
			TileLayer& Layer = mLayers[I];

			mSprite->SetTexture( Layer.SourceTexture );
			
			int TextureWidth = g_Hge->Texture_GetWidth( Layer.SourceTexture );
			int TilesInX = TextureWidth / (int)mTileWidth;
			
			for( int Y = 0; Y < Layer.NumTilesY; ++Y )
			{
				for( int X = 0; X < Layer.NumTilesX; ++X )
				{
					extern int g_ScreenWidth;
					extern int g_ScreenHeight;

					float TileLocationX = X * mTileWidth;
					float TileLocationY = Y * mTileWidth;

					// @TODO: Temporary culling code, exchange whenever we support scrolling tilesets
					if( TileLocationX < g_ScreenWidth && TileLocationY < g_ScreenHeight )
					{
						// Zero is reserved to no-tile
						short TileData = Layer.TileData[X+Y*Layer.NumTilesX];
						short TileId = GetTileId( TileData );
						if( TileId >= 0 )
						{
							int TileX = ( TileId ) % TilesInX;
							int TileY = ( TileId ) / TilesInX;

							mSprite->SetTextureRect( TileX * mTileWidth, TileY * mTileWidth, mTileWidth, mTileWidth );
							mSprite->Render( TileLocationX, TileLocationY );
						}
					}
				}
			}
		}
	}
}