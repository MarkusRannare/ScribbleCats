#ifndef SCRIBBLE_OBJECT_H
#define SCRIBBLE_OBJECT_H

namespace Scribble
{
	class Actor;

	class Object
	{
		public:
			virtual ~Object();

			inline Actor* GetOwner() const;
		protected:
			Object();
		protected:
			Actor* mOwner;
	};

	inline Actor* Object::GetOwner() const
	{
		return mOwner;
	}
}

#endif