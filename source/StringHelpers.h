#include <string>
#include <vector>

using namespace std;

static bool startsWith(string str1, string str2)
{
	int str1_size = str1.length();
	int str2_size = str2.length();

	if (str1_size < str2_size)
		return false;

	return str1.substr(0, str2_size).compare(str2) == 0;
}

static bool endsWith(string str1, string str2)
{
	int str1_size = str1.length();
	int str2_size = str2.length();

	if (str1_size < str2_size)
		return false;

	return str1.substr(str1_size - str2_size, str1_size).compare(str2) == 0;
}

static int firstIndexOf(string str, string sub)
{
	size_t pos = str.find(sub);

	if (pos != std::string::npos)
		return pos;

	return -1;
}

static int firstIndexOf(string str, string sub, int start)
{
	size_t pos = str.find(sub, start);

	if (pos != std::string::npos)
		return pos;

	return -1;
}

static int lastIndexOf(string str, string sub)
{
	int pos = str.rfind(sub);

	if (pos != std::string::npos)
		return pos;

	return -1;
}

static int lastIndexOf(string str, string sub, int end)
{
	int pos = str.rfind(sub, end);

	if (pos != std::string::npos)
		return pos;

	return -1;
}

static int nthIndexOf(string str, string sub, int n, int start)
{
	int pos = 0;
	int prevPos = -1;

	int i = 0;
	while (i < n)
	{
		pos = str.find_first_of(sub, start + pos);
		
		if (pos == std::string::npos)
			return prevPos;

		prevPos = pos;
		++i;
	}

	return pos;
}

// Replaces the last instance of org in str to rep
static string rReplace(string str, string org, string rep)
{
	int pos = lastIndexOf(str, org);

	if (pos == -1)
		return str;

	str.replace(pos, org.length(), rep);

	return str;
}

// Replaces the last instance of org in str to rep.
// Replaces the substring from org to the end of str.
static string replaceEnd(string str, string org, string rep)
{
	int pos = lastIndexOf(str, org);

	if (pos == -1)
		return str;

	str.replace(pos, str.length() - pos, rep);

	return str;
}

static string pathWithoutFilename(string str)
{
	string path = str;

	// find last "/"
	size_t index = path.rfind('/');

	if (index == std::string::npos)
		index = -1;
	
	return path.erase(index, path.length() - index);
}

static string basename(string str, char slash)
{
	// find last "/"
	size_t basename = str.rfind(slash);

	if (basename == str.length() - 1)
	{
		str.erase(str.length() - 1);
		basename = str.rfind(slash);
	}
	
	if (basename == std::string::npos)
		basename = -1;

	return str.substr(basename + 1, str.length() - (basename + 1));
}

static string basename(string str)
{
	return basename(str, '/');
}

static string basenameBackslash(string str)
{
	return basename(str, '\\');
}

static string basenameNoExt(string str)
{
	string base = basename(str);
	return base.erase(base.length() - 4);
}

static string basenameBackslashNoExt(string str)
{
	string base = basenameBackslash(str);
	return base.erase(base.length() - 4);
}

static string headname(string str, int start)
{
	// find first "/"
	if (start <= str.length())
	{
		size_t headname = str.find_first_of("/", start);

		if (headname == std::string::npos)
			headname = str.length();

		return str.substr(start, headname - start);
	}

	return "";
}

/// <summary>
/// Splits a path into parts using the "/"s inside it
/// </summary>
/// <param name="path">path to split</param>
/// <param name="maxNumOfDirs">maximum number of splits to have</param>
/// <returns>array of split indices</returns>
static vector<int> splitPath(string path, int start, int maxNumOfSplits)
{
	int max = maxNumOfSplits + 1;
	vector<int> parts(max);

	parts[0] = start;

	// Fill parts with indices of "/"s
	for (int i = 1; i < max; i++)
	{
		if (parts[i - 1] == -1)
			parts[i] = -1;
		else
			parts[i] = firstIndexOf(path, "/", parts[i - 1] + 1);
	}

	// Set i to be the last "/" found or the first -1 given
	int i = 0;
	while (i < max && parts[i] != -1)
		++i;

	// If i was not the last "/" found, set it to the index of the last char
	if (parts[i] == -1)
		parts[i] = path.length();
	
	return parts;
}