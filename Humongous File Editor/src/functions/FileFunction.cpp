#include "functions/FileFunction.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		LPCWSTR FileFunction::getFilter()
		{
			return L"\
						Humongous Game Files (*.HE0, *.HE2, *.HE3, *.HE4, *.(A))\
						\0*.HE0;*.he0;*.HE2;*.he2;*.HE3;*.he3;*.HE4;*.he4;*.(A);*.(a)\0\
						Humongous A Files (*.(A))\
						\0*.(A);*.(a)\0\
						Humongous Talkie Files (*.HE2)\
						\0*.HE2;*.he2\0\
						Humongous Song Files (*.HE4)\
						\0*.HE4;*.he4\0";
		}
	}
}