#ifndef SCRIBBLE_GUICONTAINER_H
#define SCRIBBLE_GUICONTAINER_H

#include "collection_types.h"
#include "Vector2.h"

namespace Scribble
{
	class GUIHandler;

	class GUIContainer
	{
		public:
			GUIContainer( GUIHandler* Handler, const Vector2& Location );
			virtual ~GUIContainer();

			virtual void Enter();
			virtual void Exit();

			void AddSubContainer( GUIContainer* Container );
			void RemoveSubContainer( GUIContainer* Container );

			virtual void Tick( float DeltaTime );
			virtual void Render();
		protected:
			GUIHandler* mHandler;
			Vector2 mLocation;
		private:
			foundation::Array<GUIContainer*> mSubContainers;
	};
};

#endif