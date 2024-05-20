//
// Created by sr-71 on 01/05/2024.
//

#include "FileDownloader.hpp"

FileDownloader::FileDownloader()
{
	curl_global_init(CURL_GLOBAL_ALL);
	_curl_handle = curl_easy_init();
}

FileDownloader::~FileDownloader()
{
	curl_easy_cleanup(_curl_handle);
	curl_global_cleanup();
}

void FileDownloader::setProgressCallback(std::function<void(double)> callback) { _progress_callback = callback; }

void FileDownloader::setCompletionCallback(std::function<void()> callback) { _completion_callback = callback; }
void FileDownloader::setErrorCallback(std::function<void(const std::string &)> callback) { _error_callback = callback; }

void FileDownloader::downloadFile(const std::string &url, const std::string &destination)
{
	if (_curl_handle)
	{
		FILE *file = fopen(destination.c_str(), "wb");
		if (file)
		{
			curl_easy_setopt(_curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);

			curl_easy_setopt(_curl_handle, CURLOPT_URL, url.c_str());
			curl_easy_setopt(_curl_handle, CURLOPT_WRITEDATA, file);
			curl_easy_setopt(_curl_handle, CURLOPT_WRITEFUNCTION, writeCallback);

			curl_easy_setopt(_curl_handle, CURLOPT_NOPROGRESS, 0L);
			curl_easy_setopt(_curl_handle, CURLOPT_XFERINFOFUNCTION, progressCallback);
			curl_easy_setopt(_curl_handle, CURLOPT_XFERINFODATA, this);

			CURLcode res = curl_easy_perform(_curl_handle);
			fclose(file);
			if (res != CURLE_OK)
			{
				std::cerr << "Erreur lors du téléchargement : " << curl_easy_strerror(res) << std::endl;
				if (_error_callback)
					_error_callback(curl_easy_strerror(res));
			}
			else if (_completion_callback)
				_completion_callback();
		}
		else
		{
			std::cerr << "Impossible d'ouvrir le fichier pour l'écriture." << std::endl;
			if (_error_callback)
				_error_callback("Impossible d'ouvrir le fichier pour l'écriture.");
		}
	}
}

int FileDownloader::progressCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t, curl_off_t)
{
	FileDownloader *downloader = static_cast<FileDownloader *>(clientp);
	if (downloader && downloader->_progress_callback)
	{
		double progress = dltotal > 0 ? static_cast<double>(dlnow) / static_cast<double>(dltotal): 0.0;
		downloader->_progress_callback(progress);
	}
	return 0;
}

size_t FileDownloader::writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	return fwrite(contents, size, nmemb, static_cast<FILE *>(userp));
}
