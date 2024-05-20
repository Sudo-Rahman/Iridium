//
// Created by sr-71 on 01/05/2024.
//

#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <curl/curl.h>

class FileDownloader
{
public:
	FileDownloader();

	~FileDownloader();

	/**
	 * @brief Set the progress callback
	 * @param callback the callback to call when the progress is updated (double between 0 and 1)
	 */
	void setProgressCallback(std::function<void(double)> callback);

	void setCompletionCallback(std::function<void()> callback);

	void setErrorCallback(std::function<void(const std::string &)> callback);

	void downloadFile(const std::string &url, const std::string &destination);


private:
	CURL *_curl_handle = nullptr;
	std::function<void(double)> _progress_callback = nullptr;
	std::function<void()> _completion_callback = nullptr;
	std::function<void(const std::string &)> _error_callback = nullptr;

	static int progressCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t /*ultotal*/,
	                            curl_off_t /*ulnow*/);

	static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);
};
