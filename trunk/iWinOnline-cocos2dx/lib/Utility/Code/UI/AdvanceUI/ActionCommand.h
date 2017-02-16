#ifndef __ACTION_COMMAND_H__
#define __ACTION_COMMAND_H__

#include "cocos2d.h"

#include "../../LoadDataXML.h"
#include "RKTool_Code/RKList.h"
#include "RKBaseLog/Debug.h"
#define MANAGE_ACTION 0
#ifdef OS_ANDROID_
#define USE_JSONCPP 1 
#else
#define USE_JSONCPP 1 
#endif
#if USE_JSONCPP == 1
#include "../../json/include/json/json.h"
#define JSON_VALUE Json::Value
#define JSON_READER Json::Reader
#define JSON_READER_PARSE(reader , str, value) reader.parse(str, value)
#define JSON_SIZE_VALUE(value) value.size()
#define JSON_ERROR_CODE(reader) reader.getFormatedErrorMessages()

#else
#include "external/json/rapidjson.h"
#include "external/json/document.h"
#include "external/json/reader.h"
#define JSON_VALUE rapidjson::Document
#define JSON_READER rapidjson::Reader
#define JSON_READER_PARSE(reader , str, value) reader.Parse(str, value)
#define JSON_SIZE_VALUE(value) value.Size()
#define JSON_ERROR_CODE(reader) std::to_string(reader.GetParseErrorCode())
#endif

using namespace RKUtils;
using namespace ui;
namespace Utility
{
	namespace UI_Widget
	{	
		class ActionCommand
		{
		private:

			struct ActionDetail
			{
				xml::BasicDec * value_action;
				std::string		name_action;
				float			time_action;

				xml::BasicDec *	_hook_value;
			public:
				ActionDetail()
				{
					value_action = new xml::BasicDec();
					_hook_value = nullptr;
					name_action = "";
					time_action = 1.0f;
				}

				ActionDetail * clone();

				virtual ~ActionDetail()
				{
					if (value_action)
					{
						delete value_action;
						value_action = nullptr;
					}
					if (_hook_value)
					{
						delete _hook_value;
						_hook_value = nullptr;
					}

					name_action = "";
					time_action = 1.0f;
				}

				FiniteTimeAction * GetAction(int & action, cocos2d::Node * it_res, cocos2d::Node * parent, bool parent_stick, bool IsLayer = false);

				void ParseActionDetail(const JSON_VALUE value,const RKString & name);

				void SetValueHook(xml::BasicDec value, const RKString & name_value);

				void ClearHookCommand(const RKString & name_value = "");

				void SetTimeAction(float time) { time_action = time; }

				xml::BasicDec * GetValueAction() {return value_action;}
			};

			struct ActionRepeat
			{
				std::vector<std::vector<ActionDetail*>> p_list_action_;
				int number_repeat;

				ActionRepeat()
				{
					p_list_action_.clear();
					number_repeat = 1;
				}

				ActionRepeat * clone();

				virtual ~ActionRepeat()
				{
					number_repeat = 0;
					for (auto ls : p_list_action_)
					{
						for (auto ac : ls)
						{
							delete ac;
						}
						ls.clear();
					}
					p_list_action_.clear();
				}

			};

			std::map<RKString, ActionRepeat*> p_list_actions;

			JSON_VALUE JRootValue;

			ActionRepeat * ReadJsonAction(const  JSON_VALUE value);

			ActionDetail* ParseDetailJsonAction(const JSON_VALUE value);

			Vector<FiniteTimeAction*> GetSquenceActionDetail(const RKString & name, int idx, int & action_tag, cocos2d::Node * it_res, cocos2d::Node * parent, bool parent_stick, bool IsLayer = false) const;

		public:
			ActionCommand();

			virtual ~ActionCommand();

			bool  ReadJsonString(const char * jstr);

			Vector<FiniteTimeAction*> GetSequenceActionData(int & action_tag, cocos2d::Node * it_res, cocos2d::Node * parent,int & flag, bool parent_stick , bool IsLayer = false) const;

			ActionCommand * Clone();

			ActionDetail * GetActionDetail(const RKString &  name_act, int idx_squence, int idx_detail);

			int GetNumberDetailActInSquence(const RKString &  act , int squence);

			int GetNumberSquenceInAct(const RKString &  act);

			std::vector<RKString> GetNumberActInCmd();
		};

	}
}

#endif //__ACTION_COMMAND_H__

