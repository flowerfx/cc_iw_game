#ifndef _DOWNLOAD_HANDLER_H_
#define _DOWNLOAD_HANDLER_H_


#include "../../DisplayView.h"
#include "../../FileManager.h"

#include "RKThread_Code/RKThreadWrapper.h"
#include "RKThread_Code/RKInterlock.h"

//#define USE_HTTP_REQUEST

#ifndef USE_HTTP_REQUEST
#include "network/CCDownloader.h"
#else
#include "network/HttpClient.h"
#endif
using namespace cocos2d;
using namespace network;
using namespace RKUtils;
namespace Utility
{
	namespace Social
	{
		namespace Downloader
		{
#define NUMBER_DOWNLOAD_MAX 5
			class DownloadHandler
			{
			private:

				struct state_save_url
				{
					RKString url_download;
					RKString name_save;
					std::function<void(void * data, void * str, int tag)> call_back;

					state_save_url()
					{
						url_download = "";
						name_save = "";
						call_back = nullptr;
					}
					state_save_url(RKString p_url_download, RKString p_name_save)
					{
						url_download = p_url_download;
						name_save = p_name_save;
						call_back = nullptr;
					}
					state_save_url(RKString p_url_download, RKString p_name_save, const std::function<void(void * data, void * str, int tag)> & func)
					{
						url_download = p_url_download;
						name_save = p_name_save;
						call_back = func;
					}

				};

				std::vector<state_save_url> p_list_url_parallel_download;

				std::vector<state_save_url> p_list_queue_url_download[NUMBER_DOWNLOAD_MAX];
				RKUtils::ThreadWrapper * p_ThreadUpdateDownload[NUMBER_DOWNLOAD_MAX];
				int	GetIdxThreadWithID(size_t id);

				void downLoadObject(RKString url, RKString nameSave, std::function<void(void * data, void * str, int tag)>  call_back , int idx_download , bool isSequence = true);
				RKUtils::RKCriticalSection*   m_callback_mutex;
#ifndef USE_HTTP_REQUEST
				//download sequence
				std::unique_ptr<network::Downloader> p_downloader[NUMBER_DOWNLOAD_MAX];
				//download parallel
				std::map<std::string, network::Downloader* > p_list_downloader;
#else
				void onObjectDownLoaded(HttpClient* pSender, HttpResponse* pResponse);
#endif
			public:
				DownloadHandler();
				~DownloadHandler();

				void initDownloader();

				void OnDownloadObjectparallel(RKString url, RKString name_save, const std::function<void(void * data, void * str, int tag)> & call_back);
				void OnDownloadObject(RKString ListUrl, RKString ListnameSave , const std::function<void(void * data, void * str, int tag)> & call_back);
			};
		}
	}
}

#endif//_DOWNLOAD_HANDLER_H_