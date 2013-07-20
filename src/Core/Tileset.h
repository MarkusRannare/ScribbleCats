#ifndef SCRIBBLE_TILESET_H
#define SCRIBBLE_TILESET_H

#include "Actor.h"
#include <Core/collection_types.h>
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

	class Tileset : public Actor
	{
		public:
			Tileset( Vector2 Location );
			~Tileset();

			void AddLayer( int NumTilesX, INT NumTilesY, float TileWidth, short* TileData, HTEXTURE SourceTexture );

			virtual void Render();

			inline short GetTileId( short TileData )
			{
				return ( TileData >> 5 ) - 1;
			}
		protected:
			void CalculateCollisionData( int NumTilesX, int NumTilesY, short* TileData, TileLayer& Layer );
		private:
			Tileset(); // No default constructor
		// @TODO: Make the private part of the tileset private again
		public:
			foundation::Array< TileLayer > mLayers;
			float mTileWidth;
			hgeSprite* mSprite;
	};
}

#endif