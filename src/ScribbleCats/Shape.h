#ifndef SCRIBBLE_SHAPE_H
#define SCRIBBLE_SHAPE_H

#include <hge/hgevector.h>

namespace Scribble
{
	class Shape
	{
		public:
			enum EType
			{
				T_Circle = 0,
				T_TypeCount = 1
			};
		public:
			virtual ~Shape();

			 EType GetType() const;

			 virtual bool TestPoint( hgeVector Point ) = 0;
		private:
			EType mType;
	};

	Shape::EType Shape::GetType() const
	{
		return mType;
	}
}

#endif