#include <System/Diagnostics/ServiceManager.hpp>

#include <System/Text/Encoding.hpp>

#include <Windows/Helpers.hpp>

namespace System
{
	namespace Diagnostics
	{
		BOOL __stdcall StopDependentServices(SC_HANDLE schSCManager, SC_HANDLE schService)
		{
			DWORD i;
			DWORD dwBytesNeeded;
			DWORD dwCount;

			LPENUM_SERVICE_STATUS   lpDependencies = NULL;
			ENUM_SERVICE_STATUS     ess;
			SC_HANDLE               hDepService;
			SERVICE_STATUS_PROCESS  ssp;

			DWORD dwStartTime = GetTickCount();
			DWORD dwTimeout = 30000; // 30-second time-out

			// Pass a zero-length buffer to get the required buffer size.
			if (EnumDependentServices(schService, SERVICE_ACTIVE, lpDependencies, 0, &dwBytesNeeded, &dwCount))
			{
				// If the Enum call succeeds, then there are no dependent
				// services, so do nothing.
				return TRUE;
			}
			else
			{
				if (GetLastError() != ERROR_MORE_DATA)
					return FALSE; // Unexpected error

				// Allocate a buffer for the dependencies.
				lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);

				if (!lpDependencies)
					return FALSE;

				__try {
					// Enumerate the dependencies.
					if (!EnumDependentServices(schService, SERVICE_ACTIVE, lpDependencies, dwBytesNeeded, &dwBytesNeeded, &dwCount))
						return FALSE;

					for (i = 0; i < dwCount; i++)
					{
						ess = *(lpDependencies + i);
						// Open the service.
						hDepService = OpenService(schSCManager, ess.lpServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);

						if (!hDepService)
							return FALSE;

						__try {
							// Send a stop code.
							if (!ControlService(hDepService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp))
								return FALSE;

							// Wait for the service to stop.
							while (ssp.dwCurrentState != SERVICE_STOPPED)
							{
								Sleep(ssp.dwWaitHint);

								if (!QueryServiceStatusEx(hDepService,
									SC_STATUS_PROCESS_INFO,
									(LPBYTE)&ssp,
									sizeof(SERVICE_STATUS_PROCESS),
									&dwBytesNeeded))
									return FALSE;

								if (ssp.dwCurrentState == SERVICE_STOPPED)
									break;

								if (GetTickCount() - dwStartTime > dwTimeout)
									return FALSE;
							}
						}
						__finally
						{
							// Always release the service handle.
							CloseServiceHandle(hDepService);
						}
					}
				}
				__finally
				{
					// Always free the enumeration buffer.
					HeapFree(GetProcessHeap(), 0, lpDependencies);
				}
			}

			return TRUE;
		}


		ServiceManager::~ServiceManager()
		{
		}

		bool ServiceManager::Install(const std::wstring& serviceName, const std::wstring& displayName, const std::wstring& file, const std::wstring& dependencies/* = L""*/)
		{
			bool ret = false;
			SC_HANDLE schSCManager = nullptr;

			// Open the local default service control manager database 
			schSCManager = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
			if (schSCManager != nullptr)
			{
				// Install the service into SCM by calling CreateService 
				SC_HANDLE schService = ::CreateService(
					schSCManager,                   // SCManager database 
					serviceName.c_str(),			// Name of service 
					displayName.c_str(),			// Name to display 
					SERVICE_ALL_ACCESS,				// Desired access 
					SERVICE_WIN32_OWN_PROCESS,      // Service type 
					SERVICE_AUTO_START,				// Service start type 
					SERVICE_ERROR_NORMAL,           // Error control type 
					file.c_str(),					// Service's binary 
					nullptr,                           // No load ordering group 
					nullptr,                           // No tag identifier 
					dependencies.empty() ? nullptr : dependencies.c_str(),                // Dependencies 
					nullptr,                     // Service running account 
					nullptr                     // Password of the account 
				);

				if (schService != nullptr)
				{
					ret = true;

					CloseServiceHandle(schService);
				}

				CloseServiceHandle(schSCManager);
			}

			return ret;
		}

		bool ServiceManager::Delete(const std::wstring& serviceName)
		{
			SC_HANDLE schSCManager;
			SC_HANDLE schService;

			// Get a handle to the SCM database. 

			schSCManager = OpenSCManager(
				NULL,                    // local computer
				NULL,                    // ServicesActive database 
				SC_MANAGER_ALL_ACCESS);  // full access rights 

			if (NULL == schSCManager)
			{
				//printf("OpenSCManager failed (%d)\n", GetLastError());
				return false;
			}

			// Get a handle to the service.

			schService = OpenService(
				schSCManager,       // SCM database 
				serviceName.c_str(),          // name of service 
				DELETE);            // need delete access 

			if (schService == NULL)
			{
				//printf("OpenService failed (%d)\n", GetLastError());
				CloseServiceHandle(schSCManager);
				return false;
			}

			// Delete the service.
			bool ret = false;
			if (!DeleteService(schService))
			{
				//printf("DeleteService failed (%d)\n", GetLastError());
			}
			else {
				ret = true;
			}

			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);

			return ret;

			/*SERVICE_STATUS status = {};
			BOOL fReturn;

			// Open the local default service control manager database
			SC_HANDLE manager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
			if (manager == nullptr)
			{
			assert_msg(manager != nullptr, "OpenSCManager() failed. GetLastError() returned " << GetLastError());
			}
			else
			{
			SC_HANDLE service = OpenServiceW(manager, serviceName.c_str(), SERVICE_ALL_ACCESS);
			if (service == nullptr)
			{
			assert_msg(service != nullptr, "OpenService() failed. GetLastError() returned " << GetLastError());
			}
			else
			{
			fReturn = ControlService(service, SERVICE_CONTROL_STOP, &status);
			assert_msg(fReturn == TRUE, "ControlService() failed. GetLastError() returned " << GetLastError());

			// TODO: nezacyklit sa!!!
			while (status.dwCurrentState != SERVICE_STOPPED)
			{
			Sleep(1000);

			fReturn = QueryServiceStatus(service, &status);
			assert_msg(fReturn == TRUE, "QueryServiceStatus() failed. GetLastError() returned " << GetLastError());
			}

			fReturn = DeleteService(service);
			assert_msg(fReturn == TRUE, "DeleteService() failed. GetLastError() returned " << GetLastError());

			fReturn = CloseServiceHandle(service);
			assert_msg(fReturn == TRUE, "CloseServiceHandle() failed. GetLastError() returned " << GetLastError());
			}

			fReturn = CloseServiceHandle(manager);
			assert_msg(fReturn == TRUE, "CloseServiceHandle() failed. GetLastError() returned " << GetLastError());
			}

			return (fReturn == TRUE) ? true : false;*/
		}

		bool ServiceManager::IsInstalled(const std::wstring& serviceName)
		{
			bool fReturn = false;

			SC_HANDLE schSCManager = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
			if (schSCManager != nullptr)
			{
				SC_HANDLE schService = OpenService(schSCManager, serviceName.c_str(), GENERIC_READ);
				if (schService != nullptr) {
					fReturn = true;

					CloseServiceHandle(schService);
				}

				CloseServiceHandle(schSCManager);
			}

			return fReturn;
		}

		bool ServiceManager::Start(const std::wstring& serviceName)
		{
			bool ret = false;
			SERVICE_STATUS_PROCESS ssStatus;
			DWORD dwOldCheckPoint;
			DWORD dwStartTickCount;
			DWORD dwWaitTime;
			DWORD dwBytesNeeded;

			// Get a handle to the SCM database. 

			SC_HANDLE schSCManager = OpenSCManager(
				NULL,                    // local computer
				NULL,                    // servicesActive database 
				SC_MANAGER_ALL_ACCESS);  // full access rights 

			if (NULL == schSCManager)
			{
				//printf("OpenSCManager failed (%d)\n", GetLastError());
				return ret;
			}

			// Get a handle to the service.

			SC_HANDLE schService = OpenService(
				schSCManager,         // SCM database 
				serviceName.c_str(),            // name of service 
				SERVICE_ALL_ACCESS);  // full access 

			if (schService == NULL)
			{
				//printf("OpenService failed (%d)\n", GetLastError());
				CloseServiceHandle(schSCManager);
				return ret;
			}

			// Check the status in case the service is not stopped. 

			if (!QueryServiceStatusEx(
				schService,                     // handle to service 
				SC_STATUS_PROCESS_INFO,         // information level
				(LPBYTE)&ssStatus,             // address of structure
				sizeof(SERVICE_STATUS_PROCESS), // size of structure
				&dwBytesNeeded))              // size needed if buffer is too small
			{
				//printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
				CloseServiceHandle(schService);
				CloseServiceHandle(schSCManager);
				return ret;
			}

			// Check if the service is already running. It would be possible 
			// to stop the service here, but for simplicity this example just returns. 

			if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
			{
				//printf("Cannot start the service because it is already running\n");
				CloseServiceHandle(schService);
				CloseServiceHandle(schSCManager);
				return ret;
			}

			// Save the tick count and initial checkpoint.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;

			// Wait for the service to stop before attempting to start it.

			while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
			{
				// Do not wait longer than the wait hint. A good interval is 
				// one-tenth of the wait hint but not less than 1 second  
				// and not more than 10 seconds. 

				dwWaitTime = ssStatus.dwWaitHint / 10;

				if (dwWaitTime < 1000)
					dwWaitTime = 1000;
				else if (dwWaitTime > 10000)
					dwWaitTime = 10000;

				Sleep(dwWaitTime);

				// Check the status until the service is no longer stop pending. 

				if (!QueryServiceStatusEx(
					schService,                     // handle to service 
					SC_STATUS_PROCESS_INFO,         // information level
					(LPBYTE)&ssStatus,             // address of structure
					sizeof(SERVICE_STATUS_PROCESS), // size of structure
					&dwBytesNeeded))              // size needed if buffer is too small
				{
					//printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
					CloseServiceHandle(schService);
					CloseServiceHandle(schSCManager);

					return ret;
				}

				if (ssStatus.dwCheckPoint > dwOldCheckPoint)
				{
					// Continue to wait and check.

					dwStartTickCount = GetTickCount();
					dwOldCheckPoint = ssStatus.dwCheckPoint;
				}
				else
				{
					if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
					{
						//printf("Timeout waiting for service to stop\n");
						CloseServiceHandle(schService);
						CloseServiceHandle(schSCManager);

						return ret;
					}
				}
			}

			// Attempt to start the service.

			if (!StartService(
				schService,  // handle to service 
				0,           // number of arguments 
				NULL))      // no arguments 
			{
				//printf("StartService failed (%d)\n", GetLastError());
				CloseServiceHandle(schService);
				CloseServiceHandle(schSCManager);

				return ret;
			}
			//else printf("Service start pending...\n");

			// Check the status until the service is no longer start pending. 

			if (!QueryServiceStatusEx(
				schService,                     // handle to service 
				SC_STATUS_PROCESS_INFO,         // info level
				(LPBYTE)&ssStatus,             // address of structure
				sizeof(SERVICE_STATUS_PROCESS), // size of structure
				&dwBytesNeeded))              // if buffer too small
			{
				//printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
				CloseServiceHandle(schService);
				CloseServiceHandle(schSCManager);

				return ret;
			}

			// Save the tick count and initial checkpoint.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;

			while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
			{
				// Do not wait longer than the wait hint. A good interval is 
				// one-tenth the wait hint, but no less than 1 second and no 
				// more than 10 seconds. 

				dwWaitTime = ssStatus.dwWaitHint / 10;

				if (dwWaitTime < 1000)
					dwWaitTime = 1000;
				else if (dwWaitTime > 10000)
					dwWaitTime = 10000;

				Sleep(dwWaitTime);

				// Check the status again. 

				if (!QueryServiceStatusEx(
					schService,             // handle to service 
					SC_STATUS_PROCESS_INFO, // info level
					(LPBYTE)&ssStatus,             // address of structure
					sizeof(SERVICE_STATUS_PROCESS), // size of structure
					&dwBytesNeeded))              // if buffer too small
				{
					//printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
					break;
				}

				if (ssStatus.dwCheckPoint > dwOldCheckPoint)
				{
					// Continue to wait and check.

					dwStartTickCount = GetTickCount();
					dwOldCheckPoint = ssStatus.dwCheckPoint;
				}
				else
				{
					if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
					{
						// No progress made within the wait hint.
						break;
					}
				}
			}

			// Determine whether the service is running.

			ret = (ssStatus.dwCurrentState == SERVICE_RUNNING);

			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);

			return ret;
		}

		bool ServiceManager::IsRunning(const std::wstring& serviceName)
		{
			bool ret = false;
			SERVICE_STATUS_PROCESS serviceStatus;
			DWORD dwBytesNeeded;

			// Get a handle to the SCM database. 

			SC_HANDLE schSCManager = OpenSCManager(
				NULL,                    // local computer
				NULL,                    // servicesActive database 
				SC_MANAGER_ALL_ACCESS);  // full access rights 

			if (schSCManager != NULL)
			{
				// Get a handle to the service.

				SC_HANDLE schService = OpenService(
					schSCManager,         // SCM database 
					serviceName.c_str(),  // name of service 
					SERVICE_ALL_ACCESS);  // full access 

				if (schService != NULL)
				{
					if (QueryServiceStatusEx(
						schService,                     // handle to service 
						SC_STATUS_PROCESS_INFO,         // information level
						(LPBYTE)&serviceStatus,         // address of structure
						sizeof(SERVICE_STATUS_PROCESS), // size of structure
						&dwBytesNeeded))				// size needed if buffer is too small
					{
						ret = (serviceStatus.dwCurrentState == SERVICE_RUNNING);
					}

					CloseServiceHandle(schService);
				}

				CloseServiceHandle(schSCManager);
			}

			return ret;
		}

		bool ServiceManager::Stop(const std::wstring& serviceName)
		{
			SERVICE_STATUS_PROCESS ssp;
			DWORD dwStartTime = GetTickCount();
			DWORD dwBytesNeeded;
			DWORD dwTimeout = 30000; // 30-second time-out
			DWORD dwWaitTime;

			bool ret = false;

			// Get a handle to the SCM database. 

			SC_HANDLE schSCManager = OpenSCManager(
				NULL,                    // local computer
				NULL,                    // ServicesActive database 
				SC_MANAGER_ALL_ACCESS);  // full access rights 

			if (NULL == schSCManager)
			{
				//fwprintf(fp, L"OpenSCManager failed (%d)\n", GetLastError());
				return false;
			}

			// Get a handle to the service.

			SC_HANDLE schService = OpenService(
				schSCManager,         // SCM database 
				serviceName.c_str(),            // name of service 
				SERVICE_STOP |
				SERVICE_QUERY_STATUS |
				SERVICE_ENUMERATE_DEPENDENTS);

			if (schService == NULL)
			{
				//fwprintf(fp, L"OpenService failed (%d)\n", GetLastError());
				CloseServiceHandle(schSCManager);
				return false;
			}

			// Make sure the service is not already stopped.

			if (!QueryServiceStatusEx(
				schService,
				SC_STATUS_PROCESS_INFO,
				(LPBYTE)&ssp,
				sizeof(SERVICE_STATUS_PROCESS),
				&dwBytesNeeded))
			{
				//fwprintf(fp, L"QueryServiceStatusEx failed (%d)\n", GetLastError());
				ret = false;
				goto stop_cleanup;
			}

			if (ssp.dwCurrentState == SERVICE_STOPPED)
			{
				//fwprintf(fp, L"Service is already stopped.\n");
				ret = true;
				goto stop_cleanup;
			}

			// If a stop is pending, wait for it.

			while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
			{
				//fwprintf(fp, L"Service stop pending...\n");

				// Do not wait longer than the wait hint. A good interval is 
				// one-tenth of the wait hint but not less than 1 second  
				// and not more than 10 seconds. 

				dwWaitTime = ssp.dwWaitHint / 10;

				if (dwWaitTime < 1000)
					dwWaitTime = 1000;
				else if (dwWaitTime > 10000)
					dwWaitTime = 10000;

				Sleep(dwWaitTime);

				if (!QueryServiceStatusEx(
					schService,
					SC_STATUS_PROCESS_INFO,
					(LPBYTE)&ssp,
					sizeof(SERVICE_STATUS_PROCESS),
					&dwBytesNeeded))
				{
					//fwprintf(fp, L"QueryServiceStatusEx failed (%d)\n", GetLastError());
					ret = false;
					goto stop_cleanup;
				}

				if (ssp.dwCurrentState == SERVICE_STOPPED)
				{
					//fwprintf(fp, L"Service stopped successfully.\n");
					ret = true;
					goto stop_cleanup;
				}

				if (GetTickCount() - dwStartTime > dwTimeout)
				{
					//fwprintf(fp, L"Service stop timed out.\n");
					ret = false;
					goto stop_cleanup;
				}
			}

			// If the service is running, dependencies must be stopped first.

			StopDependentServices(schSCManager, schService);

			// Send a stop code to the service.

			if (!ControlService(
				schService,
				SERVICE_CONTROL_STOP,
				(LPSERVICE_STATUS)&ssp))
			{
				//fwprintf(fp, L"ControlService failed (%d)\n", GetLastError());
				ret = false;
				goto stop_cleanup;
			}

			// Wait for the service to stop.

			while (ssp.dwCurrentState != SERVICE_STOPPED)
			{
				Sleep(ssp.dwWaitHint);

				if (!QueryServiceStatusEx(
					schService,
					SC_STATUS_PROCESS_INFO,
					(LPBYTE)&ssp,
					sizeof(SERVICE_STATUS_PROCESS),
					&dwBytesNeeded))
				{
					//fwprintf(fp, L"QueryServiceStatusEx failed (%d)\n", GetLastError());
					ret = false;
					goto stop_cleanup;
				}

				if (ssp.dwCurrentState == SERVICE_STOPPED)
					break;

				if (GetTickCount() - dwStartTime > dwTimeout)
				{
					//fwprintf(fp, L"Wait timed out\n");
					ret = false;
					goto stop_cleanup;
				}
			}

			//fwprintf(fp, L"Service stopped successfully\n");

			ret = true;

		stop_cleanup:
			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);

			return ret;
		}

		bool ServiceManager::ConfigureFailActions(const std::wstring& serviceName)
		{
			BOOL fReturn = FALSE;

			SC_HANDLE manager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
			if (manager != nullptr)
			{
				SC_HANDLE service = OpenService(manager, serviceName.c_str(), SERVICE_ALL_ACCESS);
				if (service != nullptr)
				{
					SERVICE_FAILURE_ACTIONS servFailActions = { 0 };
					SC_ACTION failActions[3];

					failActions[0].Type = SC_ACTION_RESTART; //Failure action: Restart Service
					failActions[0].Delay = 5000; //number of seconds to wait before performing failure action, in milliseconds
					failActions[1].Type = SC_ACTION_RESTART;
					failActions[1].Delay = 5000;
					failActions[2].Type = SC_ACTION_RESTART;
					failActions[2].Delay = 5000;

					servFailActions.dwResetPeriod = 86400; // Reset Failures Counter, in Seconds = 1day
					servFailActions.lpCommand = NULL; //Command to perform due to service failure, not used
					servFailActions.lpRebootMsg = NULL; //Message during rebooting computer due to service failure, not used
					servFailActions.cActions = 3; // Number of failure action to manage
					servFailActions.lpsaActions = failActions;

					fReturn = ::ChangeServiceConfig2(service, SERVICE_CONFIG_FAILURE_ACTIONS, &servFailActions);

					CloseServiceHandle(service);
				}

				CloseServiceHandle(manager);
			}

			return (fReturn == TRUE) ? true : false;
		}

		bool ServiceManager::SetDelayedAutoStart(const std::wstring & serviceName, bool value)
		{
			bool success = false;

			SC_HANDLE manager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
			if (manager != nullptr)
			{
				SC_HANDLE service = OpenService(manager, serviceName.c_str(), SERVICE_ALL_ACCESS);
				if (service != nullptr)
				{
					SERVICE_DELAYED_AUTO_START_INFO info = { 0 };
					info.fDelayedAutostart = value ? TRUE : FALSE;

					success = ::ChangeServiceConfig2(service, SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &info) ? true : false;

					CloseServiceHandle(service);
				}

				CloseServiceHandle(manager);
			}

			return success;
		}

		std::wstring ServiceManager::GetServiceImagePath(const std::wstring& ServiceName)
		{
			// TODO: tu pouzit Registry class
			std::wstring path;
			HKEY hkey;

			std::wstring key = L"SYSTEM\\CurrentControlSet\\Services\\" + ServiceName;

			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_READ, &hkey) == ERROR_SUCCESS)
			{
				DWORD type;
				DWORD cbData;

				if (RegQueryValueEx(hkey, L"ImagePath", NULL, &type, NULL, &cbData) == ERROR_SUCCESS)
				{
					if ((type == REG_SZ) || (type == REG_EXPAND_SZ))
					{
						auto buff = new wchar_t[cbData];
						if (buff) {
							if (RegQueryValueEx(hkey, L"ImagePath", NULL, NULL, (LPBYTE)buff, &cbData) == ERROR_SUCCESS)
							{
								path = buff;
							}

							delete[] buff;
						}
					}
				}

				RegCloseKey(hkey);
			}

			return path;
		}

		DWORD ServiceManager::GetServiceProcessId(const std::wstring& serviceName)
		{
			DWORD dwProcessId = 0;
			SERVICE_STATUS_PROCESS serviceStatus;
			DWORD dwBytesNeeded;
			DWORD dwLastError = ERROR_SUCCESS;

			// Get a handle to the SCM database. 

			SC_HANDLE schSCManager = OpenSCManager(
				NULL,                    // local computer
				NULL,                    // servicesActive database 
				SC_MANAGER_ALL_ACCESS);  // full access rights 

			if (schSCManager != NULL)
			{
				// Get a handle to the service.

				SC_HANDLE schService = OpenService(
					schSCManager,         // SCM database 
					serviceName.c_str(),  // name of service 
					SERVICE_ALL_ACCESS);  // full access 

				if (schService != NULL)
				{
					if (QueryServiceStatusEx(
						schService,                     // handle to service 
						SC_STATUS_PROCESS_INFO,         // information level
						(LPBYTE)&serviceStatus,         // address of structure
						sizeof(SERVICE_STATUS_PROCESS), // size of structure
						&dwBytesNeeded))				// size needed if buffer is too small
					{
						dwProcessId = serviceStatus.dwProcessId;
					}

					CloseServiceHandle(schService);
				}
				else
				{
					dwLastError = GetLastError();
				}

				CloseServiceHandle(schSCManager);
			}
			else
			{
				dwLastError = GetLastError();
			}

			return dwProcessId;
		}

		HANDLE ServiceManager::GetServiceProcessHandle(const std::wstring& serviceName)
		{
			HANDLE hProcess = NULL;
			DWORD dwProcessId = GetServiceProcessId(serviceName);
			if (dwProcessId > 0)
			{
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
			}

			return hProcess;
		}

		bool ServiceManager::DoInstall(const std::wstring& serviceName, const std::wstring& displayName, bool delayedStart)
		{
			bool bReturn = false;

			wchar_t szFileName[MAX_PATH];

			if (::GetModuleFileNameW(NULL, szFileName, MAX_PATH))
			{
				auto serviceFile = std::wstring(szFileName);

				if (!ServiceManager::IsInstalled(serviceName))
				{
					if (ServiceManager::Install(serviceName, displayName, serviceFile))
					{
						if (ServiceManager::ConfigureFailActions(serviceName))
						{
							if (ServiceManager::SetDelayedAutoStart(serviceName, delayedStart))
							{
								if (ServiceManager::Start(serviceName))
								{
									bReturn = true;
								}
							}
						}
					}
				}
				else
				{
					if (!ServiceManager::IsRunning(serviceName))
					{
						if (ServiceManager::Start(serviceName))
						{
							bReturn = true;
						}
					}
				}
			}

			return bReturn;
		}

		bool ServiceManager::DoRestart(const std::wstring& serviceName)
		{
			bool bReturn = false;

			if (ServiceManager::IsInstalled(serviceName))
			{
				HANDLE hProcess = nullptr;

				DWORD pid = ServiceManager::GetServiceProcessId(serviceName);
				if (pid > 0)
				{
					hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
				}

				if (ServiceManager::IsRunning(serviceName))
				{
					bReturn = ServiceManager::Stop(serviceName);
				}

				if (IsValidHandle(hProcess))
				{
					DWORD dwExitCode = 0;
					if (GetExitCodeProcess(hProcess, &dwExitCode))
					{
						if (dwExitCode == STILL_ACTIVE)
						{
							DWORD dwWaitResult = WaitForSingleObject(hProcess, 20000);
							if (dwWaitResult == WAIT_TIMEOUT)
							{
								BOOL fReturn = TerminateProcess(hProcess, 10);
								if (fReturn == FALSE)
								{
									// TODO: toto je katastroficky scenar
									//fwprintf(fp, L"TerminateProcess() error %08X\n", GetLastError());
								}
								else
								{
									DWORD dwRet = 0;
									switch (dwRet = WaitForSingleObject(hProcess, 10000))
									{
									case WAIT_TIMEOUT:
										//fwprintf(fp, L"Unable to terminate process\n");
										break;

									case WAIT_OBJECT_0:
										//fwprintf(fp, L"Process terminated\n");
										break;

									default:
										//fwprintf(fp, L"Process termination failed with error %04X\n", dwRet);
										break;
									}
								}

								bReturn = (fReturn == TRUE) ? true : false;
							}
							else if (dwWaitResult != WAIT_OBJECT_0)
							{
								// TODO: toto je tiez na chuja dost, neviem co tu teraz
								//fwprintf(fp, L"WaitForSingleObject() returned %08X\n", dwWaitResult);
								/*assert_msg(0, "WaitForSingleObject() returned " << GetLastError());*/

								bReturn = false;
							}
							else if (dwWaitResult == WAIT_OBJECT_0)
							{
								//fwprintf(fp, L"Wait object 0\n");
							}
						}
					}

					CloseHandle(hProcess);
				}

				if (bReturn)
				{
					bReturn = ServiceManager::Start(serviceName);
				}
			}

			return bReturn;
		}

		bool ServiceManager::DoRemove(const std::wstring& serviceName)
		{
			bool bReturn = false;

			if (ServiceManager::IsInstalled(serviceName))
			{
				HANDLE hProcess = nullptr;

				DWORD pid = ServiceManager::GetServiceProcessId(serviceName);
				if (pid > 0)
				{
					hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
				}

				if (ServiceManager::IsRunning(serviceName))
				{
					printf("Is Running\n");
					bReturn = ServiceManager::Stop(serviceName);
				}

				if (IsValidHandle(hProcess))
				{
					printf("Handle is valid\n");

					DWORD dwExitCode = 0;
					if (GetExitCodeProcess(hProcess, &dwExitCode))
					{
						if (dwExitCode == STILL_ACTIVE)
						{
							printf("STILL_ACTIVE\n");
							DWORD dwWaitResult = WaitForSingleObject(hProcess, 20000);
							if (dwWaitResult == WAIT_TIMEOUT)
							{
								BOOL fReturn = TerminateProcess(hProcess, 10);
								if (fReturn == FALSE)
								{
									printf("TerminateProcess() error %08X\n", GetLastError());
								}
								else
								{
									DWORD dwRet = 0;
									switch (dwRet = WaitForSingleObject(hProcess, 10000))
									{
									case WAIT_TIMEOUT:
										printf("Unable to terminate process\n");
										break;

									case WAIT_OBJECT_0:
										printf("Process terminated\n");
										break;

									default:
										printf("Process termination failed with error %04X\n", dwRet);
										break;
									}
								}

								bReturn = (fReturn == TRUE) ? true : false;
							}
							else if (dwWaitResult != WAIT_OBJECT_0)
							{
								// TODO: toto je tiez na chuja dost, neviem co tu teraz
								printf("WaitForSingleObject() returned %08X\n", dwWaitResult);
								/*assert_msg(0, "WaitForSingleObject() returned " << GetLastError());*/

								bReturn = false;
							}
							else if (dwWaitResult == WAIT_OBJECT_0)
							{
								printf("Wait object 0\n");
							}
						}
					}

					CloseHandle(hProcess);
				}

				printf("Deleting\n");
				bReturn = ServiceManager::Delete(serviceName);
				printf("Deleted: %d\n", bReturn);
			}
			else
			{
				bReturn = true;
			}

			Sleep(3000);
			return bReturn;
		}
	}
}
