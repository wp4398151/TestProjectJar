////////////////////////////////////////////////////
// include the functions of the windows security.
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////

#pragma once
#include "Helper.h"

LPVOID RetrieveTokenInformation(HANDLE hToken, TOKEN_INFORMATION_CLASS infoClass, DWORD& rSize);

BOOL DisplayGroupInfo(HANDLE hToken);

BOOL DisplayUserInfo(HANDLE hToken);

BOOL DisplayTokenInformation(HANDLE hToken);

BOOL DisplayCallerAccessTokenInformation();

