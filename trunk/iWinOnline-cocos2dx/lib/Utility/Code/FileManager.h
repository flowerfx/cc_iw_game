#ifndef _FILE_MANAGER_H_
#define _FILE_MANAGER_H_

#if defined OS_IOS || defined OS_ANDROID_
#endif
#include <string>
#include "cocos2d.h"

#include "RKString_Code/RKString.h"
USING_NS_CC;
using namespace RKUtils;
#define SAVE_NAME "Data.sav"
#define SAVE_GAMEPLAY_NAME "Level.sav"
namespace Utility
{
	namespace xml 
	{
		class BasicDec;
	}
	namespace File
	{
		//define common ID save
		#define SAVE_STR_NAME			"NameDisplay"
		#define SAVE_STR_COIN			"CurCoin"
		#define SAVE_STR_UDID			"UDID"
		#define	SAVE_STR_FBID			"FacebookID"
		#define	SAVE_STR_GAMECENTERID   "GamecenterID"
		#define	SAVE_STR_GOOGLEID		"GoogleID"
		#define	SAVE_STR_REMOVE_ADS		"RemoveAds"
		#define	SAVE_STR_EN_SOUND		"EnableSound"
		#define	SAVE_STR_EN_MUSIC		"EnableMusic"
        
        #define	SAVE_STR_FB_TOKEN       "FacebookToken"
        
        #define SAVE_STR_RATING         "AppRating"

		void Initialize();
        
        void InitInstallPath();
		FILE * GetFile(const RKString & namepath, const RKString &  sub);
        FILE * GetLocalFile(const RKString &  namepath, const RKString &  sub);
		const RKString   ResolveNamePath(const RKString &  namepath);
		const RKString   ChangeExtensionName(const RKString &  namepath, const RKString &  ex);
		const RKString   GenerateMipName(const RKString &  namepath);
		const RKString   ResolveLocalNamePath(const RKString &  namepath);

		const char * GetDataFromFile(const RKString &  namepath);
		const char * GetDataFromHandleFile(FILE * fp);
        
#if defined OS_IOS
        RKString GetFullPathResourceFile_IOS(std::string filename);
        RKString GetBundlePath_IOS();
        void SetFileUtilsApple();
        RKString GetWritablePath_IOS();
#endif

		class DigiData
		{
		private:
			float data1;
			float data2;
		public:
			DigiData() { data1 = 0.f; data2 = 0.f; }
			DigiData(float p) {
				data1 = ((float)p / 2.f);
				data2 = data1;
			}
			void SetData(int p)
			{
				data1 = ((float)p / 2.f);
				data2 = data1;
			}
			float GetData()
			{
				return (data2 + data1);
			}
		};

		class SaveManager;
		class DataSave
		{
			friend class SaveManager;
		private:
			xml::BasicDec * p_data_save;
		public:
			DataSave();
			virtual ~DataSave();

			bool GetFloatByName(const RKString &  name, float & result);
			void SetFloatByName(const RKString &  name, float data);
			bool RemoveFloatByName(const RKString &  name);

			bool GetCharByName(const RKString &  name, RKString & result);
			void SetCharByName(const RKString &  name, const RKString &  data);
			bool RemoveCharByName(const RKString &  name);

			bool GetIntByName(const RKString &  name, int & result);
			void SetIntByName(const RKString &  name, int data);
			bool RemoveIntByName(const RKString &  name);
		};


		class SaveManager
		{
		private:
			static SaveManager * m_Instance;
			//
			DataSave *	p_DataSave;
			//
		public:
			static SaveManager * GetInstance()
			{
				if (!m_Instance)
				{
					m_Instance = new SaveManager();
				}
				return m_Instance;
			}

			SaveManager();
			virtual ~SaveManager();

			DataSave * GetDataSave()
			{
				return p_DataSave;
			}


			void SaveTheGameSave(const RKString &  name_save);
			void LoadTheGameSave(const RKString &  name_save);

			

		protected:

			const RKString  OnLoadDataSave(const RKString &  name_save);
			void OnSaveData(FILE * f);

			const RKString  CreateBufferSave(bool Reset = false);

			const RKString  OnEngryptString(const RKString &  in);
			const RKString  OnDegryptString(const RKString &  in);

			bool OnExtractData(const RKString &  str);

			bool LoadLevelFromBufferData(const RKString &  str);

			bool OnResetFileSile();
			void OnGenerateUserName();

			const RKString &  GetUDID();

		};
#define SaveMgr SaveManager::GetInstance()
	}

}
#endif//_FILE_MANAGER_H_