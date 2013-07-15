#include "Tileset.h"
#include <foundation/memory.h>
#include "array_functions.h"
#include <hge/hgesprite.h>
#include "Vector2.h"
#include "World.h"
#include "PhysicsTypes.h"
#include "CollisionComponent.h"

using namespace foundation;

namespace Scribble
{
	Tileset::Tileset( Vector2( Location ) ) :
		Actor( Location ),
		mLayers( memory_globals::default_allocator() ),
		mTileWidth( 0 ),
		mSprite( MAKE_NEW( memory_globals::default_allocator(), hgeSprite, NULL, 0, 0, 0, 0 ) )
	{
		mSprite->SetBlendMode( BLEND_ALPHABLEND );
	}

	Tileset::~Tileset()
	{
		for( size_t I = 0; I < array::size( mLayers ); ++I )
		{
			memory_globals::default_allocator().deallocate( mLayers[I].TileData );
		}

		MAKE_DELETE( memory_globals::default_allocator(), hgeSprite, mSprite );
	}

	//void Tileset::AddLayer( int NumTilesX, INT NumTilesY, short* TileData, HTEXTURE SourceTexture )
	void Tileset::AddLayer( int NumTilesX, INT NumTilesY, float TileWidth, short* TileData, HTEXTURE SourceTexture )
	{
		// @TODO: Tile size is currently per tileset but is set per layer, decide if each layer should have a tile width or make it per actor
		mTileWidth = TileWidth;
		
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
		// @TODO: Make simplification so that we get less physic-bodies in the tileset!
		// @TODO: Care about max size of a physics body (Box2D manual suggests 50 units as largest body, see Section 1.7 Units)
		for( int Y = 0; Y < NumTilesY; ++Y )
		{
			for( int X = 0; X < NumTilesX; ++X )
			{
				// 0 means no collision
				short TileId = GetTileId( TileData[X+Y*NumTilesX] );
				if( TileId >= 0 )
				{
					CollisionComponent* ColComp = CollisionComponent::CreateRectangle( 
						b2_staticBody, 
						Vector2( X * mTileWidth + mTileWidth / 2.0f, Y * mTileWidth + mTileWidth / 2.0f ), 
						mTileWidth, 
						mTileWidth );

					AttachComponent( ColComp );
				}
			}
		}
	}

	void Tileset::Render()
	{
		extern HGE* g_Hge;

		// @TODO: Make a tileset rendering component that handles all the heavy lifting
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