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

	void Expand( Tileset* Tileset, short* TileData, int NumTilesX, int NumTilesY, int CurrX, int CurrY, int& LargestAreaX, int& LargestAreaY )
	{
		int LargestArea = 0;
		int LargestX = NumTilesX;
		int LargestY = NumTilesY;

		// Worst case, search to the bottom of the tileset
		for( int Y = CurrY; Y < NumTilesY; ++Y )
		{
			// New row and no collision, now we can stop
			if( Tileset->GetTileId( TileData[CurrX+Y*NumTilesX] ) < 0 )
			{
				break;
			}

			// Store the X that is the smallest when expanding, so that we don't search unnessesary large areas
			int ThisRoundsBestX = LargestX;
			for( int X = CurrX; X < LargestX; ++X )
			{
				short TileID = Tileset->GetTileId( TileData[X+Y*NumTilesX] );

				if( TileID >= 0 )
				{
					int CurrentTilesX = X - CurrX + 1;
					int CurrentTilesY = Y - CurrY + 1;

					int Area = CurrentTilesX * CurrentTilesY;
					if( Area > LargestArea )
					{
						LargestArea = Area;
						LargestAreaX = X;
						LargestAreaY = Y;
					}
				}
				else
				{
					ThisRoundsBestX = X;
					// We found end of line, then break;
					break;
				}
			}
			if( ThisRoundsBestX < LargestX )
			{
				LargestX = ThisRoundsBestX;
			}
		}
	}

	void Tileset::CalculateCollisionData( int NumTilesX, int NumTilesY, short* TileData, TileLayer& Layer )
	{
		// @TODO: Care about max size of a physics body (Box2D manual suggests 50 units as largest body, see Section 1.7 Units)
		const int TileDataSize = NumTilesX * NumTilesY * sizeof(short);
		short* TileDataCopy = (short*)memory_globals::default_scratch_allocator().allocate( TileDataSize );
		memcpy_s( TileDataCopy, TileDataSize, TileData, TileDataSize );

		for( int Y = 0; Y < NumTilesY; ++Y )
		{
			for( int X = 0; X < NumTilesX; ++X )
			{
				// < 0 means no collision
				short TileId = GetTileId( TileDataCopy[X+Y*NumTilesX] );
				if( TileId >= 0 )
				{
					int LargestX, LargestY;
					// Expands a rectangle down right and tries to find a maximum area
					Expand( this, TileDataCopy, NumTilesX, NumTilesY, X, Y, LargestX, LargestY );

					Vector2 UpperLeftCorner = Vector2( X * mTileWidth, Y * mTileWidth);
					Vector2 LowerRightCorner = Vector2( LargestX * mTileWidth + mTileWidth, LargestY * mTileWidth + mTileWidth );

					Vector2 Center = ( UpperLeftCorner + LowerRightCorner ) / 2.0f;
					float Width = LowerRightCorner.X - UpperLeftCorner.X;
					float Height = LowerRightCorner.Y - UpperLeftCorner.Y;

					CollisionComponent* ColComp = CollisionComponent::CreateRectangle( 
						b2_staticBody, 
						Center, 
						Width, 
						Height );

					AttachComponent( ColComp );

					// Zero the data so that we don't create collision from the expanded data
					for( int WipeY = Y; WipeY <= LargestY; ++WipeY )
					{
						for( int WipeX = X; WipeX <= LargestX; ++WipeX )
						{
							TileDataCopy[WipeX+WipeY*NumTilesX] = 0;
						}
					}

					// Skip a few tiles that we know the result for
					X = LargestX + 1;
				}
			}
		}

		memory_globals::default_scratch_allocator().deallocate( TileDataCopy );
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