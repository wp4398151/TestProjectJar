/**
 * @file 
 * @brief provide the common operation of registry.
 *
 * @author Wupeng <mailto:4398151@qq.com>
 *
 */
#pragma once
#include <windows.h>
#include <string>

class RegistryOp
{
public:
	RegistryOp(HKEY hKey = HKEY_LOCAL_MACHINE) :m_hKeyClass(hKey), m_hKey(0){}
	virtual ~RegistryOp(){ Close(); }

public:
	BOOL SaveKey(LPWSTR lpFileName);
	BOOL RestoreKey(LPWSTR lpFileName);
	BOOL Read(LPWSTR lpValueName, std::wstring& rStr);
	BOOL Read(LPWSTR lpValueName, DWORD& rdwVal);
	BOOL Read(LPWSTR lpValueName, int& rnVal);

	BOOL Write(LPCWSTR lpSubKey, LPCWSTR lpwVal);
	BOOL Write(LPWSTR lpSubKey, DWORD dwVal);
	BOOL Write(LPWSTR lpSubKey, int nVal);
	BOOL DeleteKey(HKEY hKey, LPWSTR lpwSubKey);
	BOOL DeleteValue(LPWSTR lpwValueName);
	BOOL Open(LPWSTR lpwSubKey);
	BOOL CreateKey(LPWSTR lpwSubKey);
	void Close();

protected:
	HKEY m_hKeyClass;
	HKEY m_hKey;
};