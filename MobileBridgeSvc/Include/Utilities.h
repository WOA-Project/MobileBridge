#pragma once
class Utilities
{
public:
	static void RegSetValueAndCreateKeyIfNeeded(const char* key, const char* valueName, const char* value);
	static int DoesDirectoryExist(const char* dirName);
	static int DoesFileExist(const char* filName);
};