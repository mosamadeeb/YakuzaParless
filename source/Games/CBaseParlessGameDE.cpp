#include "CBaseParlessGameDE.h"
#include "StringHelpers.h"

std::string CBaseParlessGameDE::translate_path(std::string path, int indexOfData)
{
	path = translate_path_original(path, indexOfData);

	if (firstIndexOf(path, "data/entity", indexOfData) != -1 && endsWith(path, ".txt"))
	{
		string loc = "/ja/";
		path = rReplace(path, loc, "/");
	}

	return path;
}