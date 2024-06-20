#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")

#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <lm.h>
#include <string>
#include <napi.h>

using namespace Napi;

Number roleCheck(const CallbackInfo &info)
{
    Env env = info.Env();
    std::u16string toconvert = info[0].As<String>().Utf16Value();
    std::wstring userName = std::wstring(toconvert.begin(), toconvert.end());
    DWORD result = 3;

    LPUSER_INFO_11 pBuf = NULL;
    LPUSER_INFO_11 pTmpBuf;
    DWORD dwLevel = 11;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i;
    DWORD dwTotalCount = 0;
    NET_API_STATUS nStatus;
    LPTSTR pszServerName = NULL;

    do
    {
        nStatus = NetUserEnum((LPCWSTR)pszServerName,
                              dwLevel,
                              FILTER_NORMAL_ACCOUNT,
                              (LPBYTE *)&pBuf,
                              dwPrefMaxLen,
                              &dwEntriesRead,
                              &dwTotalEntries,
                              &dwResumeHandle);
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
        {
            if ((pTmpBuf = pBuf) != NULL)
            {
                for (i = 0; (i < dwEntriesRead); i++)
                {
                    assert(pTmpBuf != NULL);

                    if (pTmpBuf == NULL)
                    {
                        fprintf(stderr, "An access violation has occurred\n");
                        break;
                    }

                    if (pTmpBuf->usri11_name == userName)
                    {
                        result = pTmpBuf->usri11_priv;
                    }

                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        }
        else
            fprintf(stderr, "A system error has occurred: %d\n", nStatus);

        if (pBuf != NULL)
        {
            NetApiBufferFree(pBuf);
            pBuf = NULL;
        }
    }

    while (nStatus == ERROR_MORE_DATA);

    if (pBuf != NULL)
        NetApiBufferFree(pBuf);

    return Number::New(env, static_cast<double>(result));
}

Object Init(Env env, Object exports)
{
    exports.Set(String::New(env, "roleCheck"),
                Function::New(env, roleCheck));
    return exports;
}

NODE_API_MODULE(roleCheck, Init)