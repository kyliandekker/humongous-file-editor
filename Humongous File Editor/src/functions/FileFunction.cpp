#include "functions/FileFunction.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		LPCWSTR FileFunction::getFilter()
		{
			return L"\
						Humongous Game Files (*.HE0, *.HE2, *.HE3, *.HE4, *.(A))\
						\0*.HE0;*.HE2;*.HE3;*.HE4;*.(A)\0\
						Humongous A Files (*.(A))\
						\0*.(A)\0\
						Humongous Talkie Files (*.HE2)\
						\0*.HE2\0\
						Humongous Song Files (*.HE4)\
						\0*.HE4\0";
		}
	}
}