////////////////////////////////////////////////////
// include the functions of the windows security.
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////

#include <Windows.h>
#include <assert.h>
#include <Sddl.h>
#include "SecurityHelper.h"

LPVOID RetrieveTokenInformation(HANDLE hToken,
	TOKEN_INFORMATION_CLASS infoClass,
	DWORD& rSize)
{
	assert(hToken);
	LPVOID pInfo = NULL;
	GetTokenInformation(hToken, infoClass, NULL, 0, &rSize);
	if (rSize == 0)
	{
		DOLOG("GetTokenInformation GetSize Failed!");
		return NULL;
	}
	pInfo = malloc(rSize);

	if (TRUE != GetTokenInformation(hToken, infoClass, pInfo, rSize, &rSize))
	{
		DOLOG("GetTokenInformation Failed!");
		return NULL;
	}
	return pInfo;
}

BOOL DisplayGroupInfo(HANDLE hToken)
{
	BOOL bResult = FALSE;
	do{
		TOKEN_GROUPS *pGroupsInfo = NULL;
		DWORD dwSize = 0;
		pGroupsInfo = (TOKEN_GROUPS*)RetrieveTokenInformation(hToken, TokenGroups, dwSize);
		if (!pGroupsInfo)
			break;
		SID_NAME_USE SidType;
		char lpName[MAX_PATH] = { 0 };
		char lpDomain[MAX_PATH] = { 0 };

		for (unsigned int i = 0; i < pGroupsInfo->GroupCount; ++i)
		{
			dwSize = MAX_PATH;
			if (!LookupAccountSidA(NULL, pGroupsInfo->Groups[i].Sid,
				lpName, &dwSize, lpDomain,
				&dwSize, &SidType))
			{
				DWORD dwResult = GetLastError();
				if (dwResult == ERROR_NONE_MAPPED)
					strcpy_s(lpName, dwSize, "NONE_MAPPED");
				else
				{
					DOLOG("LookupAccountSid Error:"+ GetLastError());
					return FALSE;
				}
			}
			DOLOG("gourpName:" + lpDomain + "\\" + lpName + ": \r\n \tenable:" + ((pGroupsInfo->Groups[i].Attributes & SE_GROUP_ENABLED) ? 1 : 0) +
				",  \tdeny_only:" + ((pGroupsInfo->Groups[i].Attributes & SE_GROUP_USE_FOR_DENY_ONLY) ? 1 : 0) + " \r\n");
		}

		free(pGroupsInfo);
		bResult = TRUE;
	} while (false);
	return bResult;
}

BOOL DisplayUserInfo(HANDLE hToken){
	BOOL bResult = FALSE;
	do{
		TOKEN_USER *pUserInfo = NULL;
		DWORD dwSize = 0;
		LPTSTR pName = NULL;//从令牌获取用户信息
		pUserInfo = (TOKEN_USER *)RetrieveTokenInformation(hToken, TokenUser, dwSize);
		if (!pUserInfo)
			break;

		SID_NAME_USE SidType;
		char lpName[MAX_PATH] = { 0 };
		char lpDomain[MAX_PATH] = { 0 };

		dwSize = MAX_PATH;
		if (!LookupAccountSidA(NULL, pUserInfo->User.Sid,
			lpName, &dwSize, lpDomain,
			&dwSize, &SidType))
		{
			int dwResult = GetLastError();
			if (dwResult == ERROR_NONE_MAPPED)
				strcpy_s(lpName, dwSize, "NONE_MAPPED");
			else
			{
				DOLOG("LookupAccountSid ErrorCode: " + GetLastError());
				free(pUserInfo);
				return FALSE;
			}
		}
		DOLOG("Current user is a member of the \r\n \t\t " + lpDomain + "\\" + lpName + " \r\n");

		free(pUserInfo);
		bResult = TRUE;
	} while (false);
	return bResult;
}

BOOL DisplayAllPrivileges(HANDLE hToken)
{
	BOOL bResult = FALSE;
	DWORD dwSize = 0;
	TOKEN_PRIVILEGES* pTokenPrivilegesInfo = NULL;
	pTokenPrivilegesInfo = (TOKEN_PRIVILEGES*)RetrieveTokenInformation(hToken, TokenPrivileges, dwSize);
	ASSERT_NOTNULLRET(pTokenPrivilegesInfo, FALSE);

	CHAR lpwNameBuf[MAX_PATH] = { 0 };
	DWORD bufSize = 0;
	LUID_AND_ATTRIBUTES *pPrivilegeAttr = NULL;
	for (UINT i = 0; i < pTokenPrivilegesInfo->PrivilegeCount; ++i)
	{
		pPrivilegeAttr = &(pTokenPrivilegesInfo->Privileges[i]);
		bufSize = MAX_PATH - 1;
		if (FALSE == LookupPrivilegeNameA("", &(pTokenPrivilegesInfo->Privileges[i].Luid), lpwNameBuf, &bufSize))
		{
			DOLOG("LookupPrivilegeNameW failed ErrorCocde: " + GetLastError());
			bResult = FALSE;
			break;
		}

		DOLOG("Privilege Name:" + lpwNameBuf + " , Enable: " + (pPrivilegeAttr->Attributes == SE_PRIVILEGE_ENABLED)
			+ ", Default Enable: " + (pPrivilegeAttr->Attributes == SE_PRIVILEGE_ENABLED_BY_DEFAULT) 
			+ ", Remove : " + (pPrivilegeAttr->Attributes == SE_PRIVILEGE_REMOVED)
			+ ", Access: " + (pPrivilegeAttr->Attributes == SE_PRIVILEGE_USED_FOR_ACCESS));
		bResult = TRUE;
	}
	free(pTokenPrivilegesInfo);

	return bResult ;
}

BOOL DisplayTokenInformation(HANDLE hToken)
{
	BOOL bResult = FALSE;
	do{
		bResult = DisplayUserInfo(hToken);
		bResult = DisplayGroupInfo(hToken);
		bResult = DisplayAllPrivileges(hToken);
	} while (false);
	return bResult;
}

BOOL DisplayCallerAccessTokenInformation()
{
	BOOL bResult = FALSE;
	do{
		HANDLE hToken = NULL;
		bResult = OpenProcessToken(GetCurrentProcess(),
			TOKEN_QUERY | TOKEN_QUERY_SOURCE, &hToken);
		if (bResult == FALSE)
		{
			DOLOG("OpenProcessToken Failed, ErrorCode: " + GetLastError());
			break;
		}
		bResult = DisplayTokenInformation(hToken);
		assert(bResult);
	} while (false);

	return bResult;
}