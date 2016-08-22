/**
* @file
* @brief provide the common operation of registry.
*
* @author Wupeng <mailto:4398151@qq.com>
*
*/

#include "RegistryOp.h"
#include "Helper.h"

void RegistryOp::Close()
{
	if (m_hKey)
	{
		RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
}

BOOL RegistryOp::SaveKey(LPWSTR lpFileName)
{
	assert(m_hKey);
	assert(lpFileName);

	long lReturn = RegSaveKeyW(m_hKey, lpFileName, NULL);

	if (lReturn == ERROR_SUCCESS)
		return TRUE;
	DOLOG("RegSaveKeyW ERROR return:" + lReturn);
	return FALSE;
}

BOOL RegistryOp::RestoreKey(LPWSTR lpFileName)
{
	assert(m_hKey);
	assert(lpFileName);
	long lReturn = RegRestoreKey(m_hKey, lpFileName, REG_WHOLE_HIVE_VOLATILE);
	if (lReturn == ERROR_SUCCESS)
	{
		return TRUE;
	}
	DOLOG("RegRestoreKey ERROR return:" + lReturn);
	return FALSE;
}

BOOL RegistryOp::Read(LPWSTR lpValueName, wstring& rStr)
{
	assert(m_hKey);
	assert(lpValueName);
	
	DWORD dwType = 0;
	DWORD dwSize = MAX_PATH;
	WCHAR szString[MAX_PATH] = {0};
	long lReturn = RegQueryValueExW(m_hKey, lpValueName, NULL, 
										&dwType, (BYTE *)szString, &dwSize);
	if (lReturn == ERROR_SUCCESS)
	{
		rStr = szString;
		return TRUE;
	}
	DOLOG("RegQueryValueExW ERROR return:" + lReturn);
	return FALSE;
}

BOOL RegistryOp::Read(LPWSTR lpValueName, DWORD& rdwVal)
{
	assert(m_hKey);
	assert(lpValueName);

	DWORD dwType = 0;
	DWORD dwSize = sizeof(DWORD);
	DWORD dwDest = 0;
	long lReturn = RegQueryValueExW(m_hKey, lpValueName, NULL, &dwType, 
									(BYTE *)&dwDest, &dwSize);
	if (lReturn == ERROR_SUCCESS)
	{
		rdwVal = dwDest;
		return TRUE;
	}
	DOLOG("RegQueryValueExW ERROR return:" + lReturn);
	return FALSE;
}

BOOL RegistryOp::Read(LPWSTR lpValueName, int& rnVal)
{
	assert(m_hKey);
	assert(lpValueName);

	DWORD dwType = 0;
	DWORD dwSize = sizeof(DWORD);
	int dwDest = 0;
	long lReturn = RegQueryValueExW(m_hKey, lpValueName, NULL, &dwType, (BYTE *)&dwDest, &dwSize);

	if (lReturn == ERROR_SUCCESS)
	{
		rnVal = dwDest;
		return TRUE;
	}
	DOLOG("RegQueryValueEx ERROR return:" + lReturn);
	return FALSE;
}

BOOL RegistryOp::Write(LPCWSTR lpValueName, LPCWSTR lpVal)
{
	assert(m_hKey);
	assert(lpValueName);
	assert(lpVal);

	// warning cbData is length of byte
	long lReturn = RegSetValueExW(m_hKey, lpValueName, 0L, REG_SZ, 
									(const BYTE *)lpVal, (wcslen(lpVal) + 1)*sizeof(WCHAR));

	if (lReturn == ERROR_SUCCESS)
		return TRUE;

	DOLOG("RegSetValueExW ERROR return:" + lReturn);
	return FALSE;
}

BOOL RegistryOp::Write(LPWSTR lpSubKey, DWORD dwVal)
{
	assert(m_hKey);
	assert(lpSubKey);

	long lReturn = RegSetValueExW(m_hKey, lpSubKey, 0L, REG_DWORD, 
									(const BYTE *)&dwVal, sizeof(DWORD));

	if (lReturn == ERROR_SUCCESS)
		return TRUE;
	DOLOG("RegSetValueExW ERROR return:" + lReturn);
	return FALSE;
}

BOOL RegistryOp::Write(LPWSTR lpwSubKey, int nVal)
{
	assert(m_hKey);
	assert(lpwSubKey);

	DWORD dwValue;
	dwValue = (DWORD)nVal;

	long lReturn = RegSetValueExW(m_hKey, lpwSubKey, 0L, REG_DWORD, 
						(const BYTE *)&dwValue, sizeof(DWORD));

	if (lReturn == ERROR_SUCCESS)
		return TRUE;

	DOLOG("RegSetValueEx ERROR return:" + lReturn);
	return FALSE;
}

BOOL RegistryOp::DeleteKey(HKEY hKey, LPWSTR lpwSubKey)
{
	assert(hKey);
	assert(lpwSubKey);

	long lReturn = RegDeleteValueW(hKey, lpwSubKey);

	if (lReturn == ERROR_SUCCESS)
	{
		return TRUE;
	}

	DOLOG("RegDeleteValue ERROR return:" + lReturn);
	return FALSE;
}

BOOL RegistryOp::DeleteValue(LPWSTR lpwValueName)
{
	assert(m_hKey);
	assert(lpwValueName);

	long lReturn = RegDeleteValueW(m_hKey, lpwValueName);

	if (lReturn == ERROR_SUCCESS)
	{
		return TRUE;
	}

	DOLOG("RegDeleteValueW ERROR return:" + lReturn);
	return FALSE;
}

BOOL RegistryOp::Open(LPWSTR lpwSubKey)
{
	assert(m_hKeyClass);
	assert(lpwSubKey);

	HKEY hKey;
	long lReturn = RegOpenKeyExW(m_hKeyClass, lpwSubKey, 0L, KEY_ALL_ACCESS, &hKey);

	if (lReturn == ERROR_SUCCESS)
	{
		m_hKey = hKey;
		return TRUE;
	}

	DOLOG("RegOpenKeyExW ERROR return:" + lReturn);
	return FALSE;
}

BOOL RegistryOp::CreateKey(LPWSTR lpwSubKey)
{
	assert(m_hKeyClass);
	assert(lpwSubKey);

	HKEY hKey;
	DWORD dw;
	long lReturn = RegCreateKeyExW(m_hKeyClass, lpwSubKey, 0L, NULL, 
									REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dw);

	if (lReturn == ERROR_SUCCESS)
	{
		m_hKey = hKey;
		return TRUE;
	}
	DOLOG("RegCreateKeyExW ERROR return:" + lReturn);
	return FALSE;
}

//WPUNITTESTSTART(RegistryOp)
//{
//	RegistryOp myRegistry(HKEY_CURRENT_USER);
//	myRegistry.Open(L"SOFTWARE\\Lenovo\\NerveCenter\\Setting");
//	int yourage = false;
//
//	EXPECT_TRUE(myRegistry.Write(L"yourage", 12));
//
//	EXPECT_TRUE(myRegistry.Read(L"yourage", yourage));
//	DOLOG("SOFTWARE\\Lenovo\\NerveCenter\\Setting\\yourage : " + yourage);
//	//DOLOG("HighQualityResolutionTimeLite Lapse: " + timelapse + "micro sec\r\n");
//	wstring name;
//	EXPECT_TRUE(myRegistry.Read(L"myname", name));
//	DOLOGW(L"SOFTWARE\\Lenovo\\NerveCenter\\Setting\\myname : " + name);
//
//	EXPECT_TRUE(myRegistry.Write(L"yourname", name.c_str()));
//
//	EXPECT_TRUE(myRegistry.DeleteValue(L"yourage"));
//
//	EXPECT_TRUE(myRegistry.CreateKey(L"SubSetting"));
//}

#include "gtest/gtest.h"

TEST(RegistryOp, TestRegistryOperation)
{
	RegistryOp myRegistry(HKEY_CURRENT_USER);
	myRegistry.Open(L"SOFTWARE\\Lenovo\\NerveCenter\\Setting");
	int yourage = false;

	EXPECT_TRUE(myRegistry.Write(L"yourage", 12));

	EXPECT_TRUE(myRegistry.Read(L"yourage", yourage));
	DOLOG("SOFTWARE\\Lenovo\\NerveCenter\\Setting\\yourage : " + yourage);
	//DOLOG("HighQualityResolutionTimeLite Lapse: " + timelapse + "micro sec\r\n");
	wstring name;
	EXPECT_TRUE(myRegistry.Read(L"myname", name));
	DOLOGW(L"SOFTWARE\\Lenovo\\NerveCenter\\Setting\\myname : " + name);

	EXPECT_TRUE(myRegistry.Write(L"yourname", name.c_str()));

	EXPECT_TRUE(myRegistry.DeleteValue(L"yourage"));

	EXPECT_TRUE(myRegistry.CreateKey(L"SubSetting"));
}
