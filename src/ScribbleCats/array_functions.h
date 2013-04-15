#ifndef SCRIBBLE_ARRAY_FUNCTIONS_H
#define SCRIBBLE_ARRAY_FUNCTIONS_H

#include <WinBase.h>
#include "foundation/array.h"

namespace foundation
{
	namespace array
	{
		template<class T>
		int find( const Array<T>& a, const T& item );

		template<class T>
		bool add_unique( Array<T>& a, const T& item );

		template<class T>
		bool remove_swap( Array<T>& a, const T& item ); 

		template<class T>
		int add_zeroed( Array<T>& a );
	}

	namespace array
	{
		template<class T>
		int find( const Array<T>& a, const T& item )
		{
			for( size_t i = 0; i < size( a ); ++i )
			{
				if( a[i] == item )
				{
					return i;
				}
			}

			return INDEX_NONE;
		}

		template<class T>
		bool add_unique( Array<T>& a, const T& item )
		{
			if( find( a, item ) == INDEX_NONE )
			{
				push_back( a, item );

				return true;
			}

			return false;
		}

		template<class T>
		bool remove_swap( Array<T>& a, const T& item )
		{
			for( size_t i = 0; i < size(a); ++i )
			{
				// Found the element, move the last element to this location in the list
				// and decrease the size
				if( a[i] == item )
				{
					// Make sure it work with 1 element
					if( size(a) == 1 )
					{
						a._size = 0;

						return true;
					}

					size_t lastIndex = size(a) - 1;
					a[i] = a[lastIndex];

					--a._size;

					return true;
				}
			}

			return false;
		}

		template<class T>
		int add_zeroed( Array<T>& a )
		{
			if( a._size == a._capacity )
			{
				grow( a );
			}

			int Index = a._size++;
			ZeroMemory( &a[Index], sizeof(T) );
			
			return Index;
		}
	}
}

#endif