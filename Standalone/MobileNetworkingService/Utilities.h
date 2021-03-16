#pragma once
class Utilities
{
public:
	static void RegSetValueAndCreateKeyIfNeeded(const wchar_t* key, const wchar_t* valueName, const char* value);
	static int DoesDirectoryExist(const wchar_t* dirName);
	static int DoesFileExist(const wchar_t* filName);
};