#include <System/Net/Http.hpp>
#include <System/Net/Url.hpp>
#include <System/IO/Path.hpp>
#include <System/Text/Encoding.hpp>

#include <Windows.h>
#include <winhttp.h>

namespace System
{
	namespace Net
	{
		namespace Http
		{
			concurrency::task<std::string> UploadFileAsync(const System::Net::Url& url, const std::string& path, const std::wstring& filePath)
			{
				return concurrency::create_task([url, path, filePath]()
				{
					std::string response;

					HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
					if (hFile != INVALID_HANDLE_VALUE)
					{
						DWORD dwFileSize = GetFileSize(hFile, nullptr);
						if (dwFileSize > 0)
						{
							auto userAgentString = L"Mozilla/5.0 (Windows NT 6.3; Trident/7.0; rv:11.0) like Gecko";

							HINTERNET hSession = WinHttpOpen(userAgentString, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, nullptr, WINHTTP_NO_PROXY_BYPASS, 0);
							if (hSession)
							{
								DWORD dwReserved = 0;

								HINTERNET hConnect = WinHttpConnect(hSession, System::Text::Encoding::ToUTF16(url.Host).c_str(), static_cast<INTERNET_PORT>(url.Port), dwReserved);
								if (hConnect)
								{
									DWORD dwFlags = WINHTTP_FLAG_REFRESH;

									if (url.Scheme == "https")
									{
										dwFlags |= WINHTTP_FLAG_SECURE;
									}

									HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", System::Text::Encoding::ToUTF16(path).c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwFlags);
									if (hRequest)
									{
										DWORD dwOptions = SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_UNKNOWN_CA;

										BOOL fSuccess = WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, reinterpret_cast<LPVOID>(&dwOptions), sizeof(dwOptions));
										if (fSuccess)
										{
											auto boundaryValue = std::to_string(GetTickCount());

											auto fileName = System::IO::Path::GetFileName(filePath);

											std::string boundaryBegin = "------ExcaliburFormBoundary" + boundaryValue + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"" + System::Text::Encoding::ToUTF8(fileName) + "\"\r\nContent-Type: application/zip\r\n\r\n";
											std::string boundaryEnd = "\r\n------ExcaliburFormBoundary" + boundaryValue + "--\r\n";

											size_t contentLength = boundaryBegin.size() + dwFileSize + boundaryEnd.size();

											fSuccess = WinHttpAddRequestHeaders(hRequest, (L"Content-Type: multipart/form-data; boundary=----ExcaliburFormBoundary" + System::Text::Encoding::ToUTF16(boundaryValue)).c_str(), DWORD(-1), WINHTTP_ADDREQ_FLAG_ADD);
											if (fSuccess)
											{
												fSuccess = WinHttpAddRequestHeaders(hRequest, (L"Content-Length: " + std::to_wstring(contentLength)).c_str(), DWORD(-1), WINHTTP_ADDREQ_FLAG_ADD);
											}

											if (fSuccess)
											{
												fSuccess = WinHttpAddRequestHeaders(hRequest, L"Connection: keep-alive", DWORD(-1), WINHTTP_ADDREQ_FLAG_ADD);
											}

											if (fSuccess)
											{
												fSuccess = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, NULL);
											}

											if (fSuccess)
											{
												DWORD dwBytesRead = 0;
												DWORD dwBytesToRead = 0;

												DWORD dwBytesWritten = 0;
												DWORD dwBytesToWrite = static_cast<DWORD>(boundaryBegin.length());

												fSuccess = (WinHttpWriteData(hRequest, boundaryBegin.c_str(), dwBytesToWrite, &dwBytesWritten) && (dwBytesToWrite == dwBytesWritten)) ? TRUE : FALSE;
												if (fSuccess)
												{
													const size_t chunkSize = 2048;

													BYTE pBuffer[chunkSize];


													while (fSuccess && ReadFile(hFile, pBuffer, ARRAYSIZE(pBuffer), &dwBytesRead, nullptr) && (dwBytesRead > 0))
													{
														fSuccess = (WinHttpWriteData(hRequest, pBuffer, dwBytesRead, &dwBytesWritten) && (dwBytesRead == dwBytesWritten)) ? TRUE : FALSE;
													}

													if (fSuccess)
													{
														dwBytesToWrite = static_cast<DWORD>(boundaryEnd.length());

														fSuccess = (WinHttpWriteData(hRequest, boundaryEnd.c_str(), dwBytesToWrite, &dwBytesWritten) && (dwBytesToWrite == dwBytesWritten)) ? TRUE : FALSE;
													}
												}

												if (fSuccess)
												{
													fSuccess = WinHttpReceiveResponse(hRequest, nullptr);
													if (fSuccess)
													{
														fSuccess = WinHttpQueryDataAvailable(hRequest, &dwBytesToRead);
														if (fSuccess)
														{
															auto pResponse = reinterpret_cast<BYTE*>(LocalAlloc(LMEM_FIXED, dwBytesToRead));
															if (pResponse)
															{
																fSuccess = (WinHttpReadData(hRequest, pResponse, dwBytesToRead, &dwBytesRead) && (dwBytesRead == dwBytesToRead)) ? TRUE : FALSE;
																if (fSuccess)
																{
																	response = std::string(reinterpret_cast<char*>(pResponse), static_cast<size_t>(dwBytesRead));
																}

																LocalFree(pResponse);
															}
														}
													}
												}
											}
										}

										WinHttpCloseHandle(hRequest);
									}

									WinHttpCloseHandle(hConnect);
								}

								WinHttpCloseHandle(hSession);
							}
						}

						CloseHandle(hFile);
					}

					return response;
				});
			}

			concurrency::task<std::string> PostRequestAsync(const System::Net::Url& url, const std::string& path, const std::string& data)
			{
				// TODO
				//	Implement exceptions on non recoverable errors
				return concurrency::create_task([url, path, data]()
				{
					std::string response;

					DWORD dwDownloaded = 0;

					// Use WinHttpOpen to obtain a session handle.
					HINTERNET hSession = WinHttpOpen(L"Excalibur Logger/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
					// Specify an HTTP server.
					if (hSession)
					{
						HINTERNET hConnect = WinHttpConnect(hSession, System::Text::Encoding::ToUTF16(url.Host).c_str(), static_cast<INTERNET_PORT>(url.Port), 0);
						if (hConnect)
						{
							// Create an HTTP request handle.
							DWORD dwFlags = 0;

							if (url.Scheme == "https")
							{
								dwFlags |= WINHTTP_FLAG_SECURE;
							}

							HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", System::Text::Encoding::ToUTF16(path).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwFlags);
							if (hRequest)
							{
								LPCWSTR szAdditionalHeaders = L"Accept: application/json\r\nContent-Type: application/json\r\n";
								DWORD dwAdditionalHeaders = static_cast<DWORD>(-1);

								// Send a request.
								DWORD dwLength = static_cast<DWORD>(data.length());
								BOOL fResult = WinHttpSendRequest(hRequest, szAdditionalHeaders, dwAdditionalHeaders, const_cast<char*>(data.c_str()), dwLength, dwLength, 0);
								if (fResult)
								{
									fResult = WinHttpReceiveResponse(hRequest, nullptr);
									// Keep checking for data until there is nothing left.
									if (fResult)
									{
										DWORD dwSize;

										do
										{
											// Check for available data.
											dwSize = 0;

											fResult = WinHttpQueryDataAvailable(hRequest, &dwSize);
											if (fResult && dwSize > 0)
											{
												// Allocate space for the buffer.
												char* pszOutBuffer = (char*)LocalAlloc(0, dwSize + 1);
												if (pszOutBuffer)
												{
													fResult = WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded);
													if (fResult)
													{
														response = response + std::string(pszOutBuffer, dwSize);
													}

													LocalFree(pszOutBuffer);
												}
												else { fResult = FALSE; }
											}

										} while (fResult && dwSize > 0);
									}
								}
								else
								{
									/*DWORD dwLastError = GetLastError();

									int a = 5;*/
								}

								WinHttpCloseHandle(hRequest);
							}

							WinHttpCloseHandle(hConnect);
						}
						else
						{
							/*DWORD dwLastError = GetLastError();

							int a = 5;*/
						}

						WinHttpCloseHandle(hSession);
					}

					return response;
				});
			}
		}
	}
}
