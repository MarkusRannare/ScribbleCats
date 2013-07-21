#ifndef SCRIBBLE_GUICONTAINER_H
#define SCRIBBLE_GUICONTAINER_H

#include "collection_types.h"

namespace Scribble
{
	class GUIHandler;

	class GUIContainer
	{
		public:
			GUIContainer( GUIHandler* Handler );
			virtual ~GUIContainer();

			virtual void Enter();
			virtual void Exit();

			void AddSubContainer( GUIContainer* Container );
			void RemoveSubContainer( GUIContainer* Container );

			virtual void Tick( float DeltaTime );
			virtual void Render();
		protected:
			GUIHandler* mHandler;
		private:
			foundation::Array<GUIContainer*> mSubContainers;
	};
};

#endif