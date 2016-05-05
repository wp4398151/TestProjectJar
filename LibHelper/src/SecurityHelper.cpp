#include <Windows.h>
#include <assert.h>
#include <Sddl.h>

//EventState evnet(L"TestEvent");
//while (true){
//	printf(" %d\r\n", evnet.GetState());
//	Sleep(1000);
//}
class EventState
{
public:
	EventState(LPWSTR lpEvnetName){
		m_hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpEvnetName);
	}
	~EventState(){
		if (m_hEvent)
		{
			CloseHandle(m_hEvent);
		}
	}
	bool GetState()
	{
		if (m_hEvent != NULL){
			DWORD res = WaitForSingleObject(m_hEvent, 0);
			if (res == WAIT_TIMEOUT){
				return true;
			}
		}
		return false;
	}
private:
	HANDLE m_hEvent;
};

LPVOID RetrieveTokenInformation(HANDLE hToken,
	TOKEN_INFORMATION_CLASS infoClass,
	DWORD& rSize)
{
	assert(hToken);
	LPVOID pInfo = NULL;
	GetTokenInformation(hToken, infoClass, NULL, 0, &rSize);
	if (rSize == 0)
	{
		return NULL;
	}
	pInfo = malloc(rSize);

	if (TRUE != GetTokenInformation(hToken, infoClass, pInfo, rSize, &rSize))
	{
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
					printf("LookupAccountSid Error %u\n", GetLastError());
					return FALSE;
				}
			}
			printf("gourpName:%s\\%s: \r\n \tenable:%d,  \tdeny_only:%d \r\n", lpDomain, lpName,
				(pGroupsInfo->Groups[i].Attributes & SE_GROUP_ENABLED) ? 1 : 0,
				(pGroupsInfo->Groups[i].Attributes & SE_GROUP_USE_FOR_DENY_ONLY) ? 1 : 0);
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
				printf("LookupAccountSid Error %u\n", GetLastError());
				free(pUserInfo);
				return FALSE;
			}
		}
		printf("Current user is a member of the \r\n \t\t %s\\%s \r\n",
			lpDomain, lpName);

		free(pUserInfo);
		bResult = TRUE;
	} while (false);
	return bResult;
}

BOOL DisplayTokenInformation(HANDLE hToken)
{
	BOOL bResult = FALSE;
	do{
		bResult = DisplayUserInfo(hToken);
		bResult = DisplayGroupInfo(hToken);
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
			int errocode = GetLastError();
			break;
		}
		bResult = DisplayTokenInformation(hToken);
		assert(bResult);
	} while (false);

	return bResult;
}