#ifndef SCRIBBLE_TILESET_H
#define SCRIBBLE_TILESET_H

#include <foundation/collection_types.h>
#include <hge/hge.h>
#include "PhysicsTypes.h"

class hgeSprite;

namespace Scribble
{
	struct TileLayer
	{
		int NumTilesX;
		int NumTilesY;

		short* TileData;

		int NumCollisionEntries;
		
		HTEXTURE SourceTexture;
	};

	class Tileset
	{
		public:
			Tileset( float TileWidth, float OriginX, float OriginY );
			~Tileset();

			void AddLayer( int NumTilesX, INT NumTilesY, short* TileData, HTEXTURE SourceTexture );

			void Tick( float CenterX, float CenterY );
			void Render();

			inline short GetTileId( short TileData )
			{
				return ( TileData >> 5 ) - 1;
			}
		protected:
			void CalculateCollisionData( int NumTilesX, int NumTilesY, short* TileData, TileLayer& Layer );
		private:
			Tileset(); // No default constructor
		private:
			foundation::Array< TileLayer > mLayers;
			float mTileWidth;
			hgeSprite* mSprite;
	};
}

#endif