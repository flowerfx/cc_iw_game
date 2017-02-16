#include "DownloadHandler.h"
#include "../../FileManager.h"

#include "../../FXFlatform/XFlatform.h"
#include "../SocialManager.h"

namespace Utility
{
	namespace Social
	{
		namespace Downloader
		{
			DownloadHandler::DownloadHandler() : 
				m_callback_mutex(0)
			{
#ifndef USE_HTTP_REQUEST
				for (int i = 0; i < NUMBER_DOWNLOAD_MAX; i++)
				{
					p_downloader[i].reset(new network::Downloader());
				}
				p_list_downloader.clear();
#endif
				if (!m_callback_mutex)
				{
					m_callback_mutex = RKCriticalSection_Create("cs_callback_mutex");
				}
			}

			DownloadHandler::~DownloadHandler()
			{
				if (m_callback_mutex)
				{
					RKCriticalSection_Destroy(&m_callback_mutex);
				}
				for (int i = 0; i < NUMBER_DOWNLOAD_MAX; i++)
				{
					delete p_ThreadUpdateDownload[i];
					p_ThreadUpdateDownload[i] = nullptr;
					p_list_queue_url_download[i].clear();
				}
				//delete[]p_downloader;
				p_list_downloader.clear();
			}

			void DownloadHandler::initDownloader()
			{
                //set init first
                for (int i = 0; i < NUMBER_DOWNLOAD_MAX; i++)
                {
                    p_ThreadUpdateDownload[i] = new ThreadWrapper();
                }
                //the run the thread
				for (int i = 0; i < NUMBER_DOWNLOAD_MAX; i++)
				{
					p_ThreadUpdateDownload[i]->CreateThreadWrapper("thread_update_downloader_" + std::to_string(i), 
					[](void * data) -> uint32
					{
                        int64_t id = -1;
#if defined OS_IOS || defined OS_ANDROID_
                        auto sid = (size_t)pthread_self();
                        id = sid;
#else
                        
						std::stringstream ss;
						ss << std::this_thread::get_id();
						id = std::stoull(ss.str());
#endif
						//auto id = std::hash<std::thread::id>()(std::this_thread::get_id());
						DownloadHandler * download_ = (DownloadHandler*)data;
						int idx = download_->GetIdxThreadWithID(id);
						if (idx < 0)
							return 1;
						download_->p_ThreadUpdateDownload[idx]->OnCheckUpdateThreadWrapper
						([download_ , idx]()
						{
							if (download_->p_list_queue_url_download[idx].size() > 0)
							{
								auto url_download = download_->p_list_queue_url_download[idx].at(0);
								download_->downLoadObject(url_download.url_download, url_download.name_save, url_download.call_back, idx, true);
							}
						});
						return 1;
					}, (void*)this, RKThreadPriority::RKThreadPriority_Lowest);
				}

				for (int i = 0; i < NUMBER_DOWNLOAD_MAX; i++)
				{
					p_list_queue_url_download[i].clear();
				}
				p_list_url_parallel_download.clear();
			}

			int	DownloadHandler::GetIdxThreadWithID(size_t id)
			{
				for (int i = 0; i < NUMBER_DOWNLOAD_MAX; i++)
				{
                    size_t cur_id = -1;
					if (p_ThreadUpdateDownload[i])
					{
                        cur_id = p_ThreadUpdateDownload[i]->GetCurrentID();
                        if(cur_id == id)
                        {
                            return i;
                        }
					}
				}
				return -1;
			}

			void DownloadHandler::OnDownloadObjectparallel(RKString url, RKString name_save, const std::function<void(void * data, void * str, int tag)> & call_back)
			{
				bool contain_url = false;
				for (auto it : p_list_url_parallel_download)
				{
					auto d_url = it.url_download;
					if (d_url == url)
					{
						contain_url = true;
						break;
					}
						
				}
				if (!contain_url)
				{
					p_list_url_parallel_download.push_back(state_save_url(url, name_save, call_back));
					downLoadObject(url, name_save, call_back , 0 , false);

				}
			}

			void DownloadHandler::OnDownloadObject(RKString ListUrl, RKString ListnameSave , const std::function<void(void * data, void * str, int tag)> & call_back)
			{
				int idx_queue_free = -1;
				for (int i = 0; i < NUMBER_DOWNLOAD_MAX; i++)
				{
					if (p_list_queue_url_download[i].size() == 0)
					{
						idx_queue_free = i;
						break;
					}
				}


                //check contain url in list queue
				bool contain_url = false;
				for (int i = 0; i < NUMBER_DOWNLOAD_MAX; i++)
				{
					for (auto it : p_list_queue_url_download[i])
					{
						auto url = it.url_download;
						if (url == ListUrl)
						{
							contain_url = true;
							break;
						}
					}
					if (contain_url)
					{
						break;
					}
				}

				//get the idx_queue with the least idx
				int idx_min = 0;
				for (int i = 0; i < NUMBER_DOWNLOAD_MAX ; i++)
				{
					if (p_list_queue_url_download[i].size() < p_list_queue_url_download[idx_min].size())
					{
						idx_min = i;
					}
				}

				if (!contain_url)
				{
					p_list_queue_url_download[idx_min].push_back(state_save_url(ListUrl, ListnameSave, call_back));
				}
				

				if (idx_queue_free > -1)
				{
					p_ThreadUpdateDownload[idx_queue_free]->OnWakeUpAndRunThread();
				}
			}


			void DownloadHandler::downLoadObject(RKString url, RKString nameSave, std::function<void(void * data, void * str, int tag)> call_back,int idx_download, bool isSequence)
			{
#ifdef USE_HTTP_REQUEST
				HttpRequest* request = new (std::nothrow) HttpRequest();
				request->setUrl(url.GetString());
				request->setRequestType(HttpRequest::Type::GET);
				const network::ccHttpRequestCallback& call_back = ([=](network::HttpClient * client, network::HttpResponse * pResponse)
				{
					onObjectDownLoaded(client, pResponse);
				});
				request->setResponseCallback(call_back);
				request->setTag(nameSave.GetString());
				HttpClient::getInstance()->send(request);                  
				request->release();
#else
				if (isSequence)
				{
					if (idx_download >= 0 && idx_download < NUMBER_DOWNLOAD_MAX)
					{
						p_downloader[idx_download]->createDownloadDataTask(url.GetString(), nameSave.GetString());
						// define success callback
						p_downloader[idx_download]->onDataTaskSuccess = [this, call_back, idx_download](const cocos2d::network::DownloadTask& task,
							std::vector<unsigned char>& data)
						{
							RKCriticalSection_Enter(m_callback_mutex);
							Image * img = new Image();
							img->initWithImageData(data.data(), data.size());
							RKString name_img = task.requestURL; //task.identifier;
							//
							if (call_back)
							{
								//RKCriticalSection_Enter(m_callback_mutex); //enter critical section

								call_back((void*)img, (void *)name_img.GetString(), 0);

								//RKCriticalSection_Leave(m_callback_mutex);
							}

							if (idx_download>= 0 && idx_download < NUMBER_DOWNLOAD_MAX &&  p_list_queue_url_download[idx_download].size() > 0)
							{
								p_list_queue_url_download[idx_download].erase(p_list_queue_url_download[idx_download].begin());//remove the url downloaded or fail ...
								if (p_list_queue_url_download[idx_download].size() > 0)
								{
									p_ThreadUpdateDownload[idx_download]->OnWakeUpAndRunThread();
									//continue download when their is the object on the list
								}
							}
							RKCriticalSection_Leave(m_callback_mutex);
						};

						p_downloader[idx_download]->onTaskError = [this , idx_download](const cocos2d::network::DownloadTask& task, int errorCode, int errorExternalCode, const std::string & error_str)
						{
							RKCriticalSection_Enter(m_callback_mutex);
							CCLOG("download sequence error with code : %d , external code: %d , msg: %s \n", errorCode, errorExternalCode, error_str.c_str());

							if(idx_download >= 0 && idx_download < NUMBER_DOWNLOAD_MAX && p_list_queue_url_download[idx_download].size() > 0)
							{
								p_list_queue_url_download[idx_download].erase(p_list_queue_url_download[idx_download].begin());//remove the url downloaded or fail ...
								if (p_list_queue_url_download[idx_download].size() > 0)
								{
									p_ThreadUpdateDownload[idx_download]->OnWakeUpAndRunThread();
									//continue download when their is the object on the list
								}
							}
							RKCriticalSection_Leave(m_callback_mutex);
							
						};
					}
				}
				else
				{
					//std::unique_ptr<network::Downloader> pdownloader;
					//pdownloader.reset(new network::Downloader());
					network::Downloader *  pdownloader = new network::Downloader();
					pdownloader->createDownloadDataTask(url.GetString(), nameSave.GetString());
					// define success callback
					pdownloader->onDataTaskSuccess = [this, call_back](const cocos2d::network::DownloadTask& task,
						std::vector<unsigned char>& data)
					{
						Image * img = new Image();
						img->initWithImageData(data.data(), data.size());
						RKString name_img = task.requestURL; //task.identifier;
												 //
						if (call_back)
						{
							//RKCriticalSection_Enter(m_callback_mutex); //enter critical section

							call_back((void*)img, (void *)name_img.GetString(), 0);

							//RKCriticalSection_Leave(m_callback_mutex);
						}
               					
						int idx = 0;
						for (auto it : p_list_url_parallel_download)
						{
							if (it.url_download == name_img)
							{
								p_list_downloader.erase(name_img.GetString());
								break;
							}
							idx++;
						}
						p_list_url_parallel_download.erase(p_list_url_parallel_download.begin() + idx);
					};

					pdownloader->onTaskError = [this, url](const cocos2d::network::DownloadTask& task, int errorCode, int errorExternalCode, const std::string & error_str)
					{
						CCLOG("download error with code : %d , external code: %d , msg: %s \n", errorCode, errorExternalCode, error_str.c_str());
						int idx = 0;
						for (auto it : p_list_url_parallel_download)
						{
							if (it.url_download == url)
							{
								p_list_downloader.erase(url.GetString());
								break;
							}
							idx++;
						}
						p_list_url_parallel_download.erase(p_list_url_parallel_download.begin() + idx);
					};
				
					p_list_downloader.insert({url.GetString(), pdownloader});
				}
#endif
			}


#ifdef USE_HTTP_REQUEST
			void DownloadHandler::onObjectDownLoaded(HttpClient* pSender, HttpResponse* pResponse)
			{
				HttpResponse* response = pResponse;

				if (!response)
				{
					SocialMgr->SetStateDownload(D_FAIL);
					cocos2d::log("No Response");
				}
				else
				{
					int statusCode = (int)response->getResponseCode();

					char statusString[64] = {};
					sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
					cocos2d::log("response code: %d", statusCode);

					if (!response->isSucceed())
					{
                        SocialMgr->SetStateDownload(D_FAIL);
						cocos2d::log("response failed");
						cocos2d::log("error buffer: %s", response->getErrorBuffer());
					}
					else
					{
						std::vector<char>*buffer = response->getResponseData();

						Image * img = new Image();
						img->initWithImageData((unsigned char *)&(buffer->front()), buffer->size());

						RKString name_img = response->getHttpRequest()->getTag();
                        
                        //
                        bool result = SocialMgr->processRegistedFunction(name_img, (void*)img, 0);  
						if (!result)
						{
							CC_SAFE_DELETE(img);
						}
						//                  
                        SocialMgr->SetStateDownload(D_SUCCESS);
					}
				}

				if (SocialMgr->GetStateDownload() == D_FAIL || SocialMgr->GetStateDownload()  == D_SUCCESS)
				{
					p_list_queue_url_download.erase(p_list_queue_url_download.begin());//remove the url downloaded or fail ...
					if (p_list_queue_url_download.size() > 0)
					{
						RKThreadCondition_WakeAll(m_conditionDownload);
                        SocialMgr->SetStateDownload(D_DOWNLOADING);
						//continue download when their is the object on the list
					}
					else
					{
						//download all succeed
						SocialMgr->SetStateDownload(D_NONE);
					}
				}
			}
#endif
		}
	}
}
