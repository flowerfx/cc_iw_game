#include "cocos2d.h"
#include "RectProcessTime.h"
#include "UI/AdvanceUI/wParticle.h"
#include "UI/AdvanceUI/wTimeProcessWidget.h"
#include "UI/AdvanceUI/wPanel.h"

RectProcessTime::RectProcessTime(WidgetEntity * panel)
{
	 _interval = 0;
	 _curent_t = 0;

	_time_bar = panel->GetWidgetChildByName("._timer");
	_particle = panel->GetWidgetChildByName(".bar_particle");
}
RectProcessTime::~RectProcessTime()
{
	_time_bar = nullptr;
	_particle = nullptr;
}

void RectProcessTime::update(float t)
{
	if (!_particle || !_time_bar || !_time_bar->Visible())
		return;

	
}
void RectProcessTime::stop()
{
	for (size_t it = 0; it < this->_time_bar->GetNumberChildWidget(); it++)
	{
		auto p = this->_time_bar->GetWidgetChildAtIdx(it);
		p->SetActionCommandWithName("BEGIN_PROCESS", CallFunc::create([it , p , this] {
			p->ForceFinishAction();
			if (it == _time_bar->GetNumberChildWidget() - 1)
			{
				_time_bar->ForceFinishAction();
				_time_bar->Visible(false);
			}
		}));
	}
	_particle->ForceFinishAction();
	_particle->Visible(false);

	_interval = 0;
	_curent_t = 0;
}
void RectProcessTime::startTime(float interval,std::function<void()> call_back)
{		
	//stop();
	_interval = interval;
	_curent_t = interval;

	xml::BasicDec dec;
	dec.InsertDatafloat("TIME", interval);
	_time_bar->SetHookCommand("PROCESS", "act0", 0, 2, dec, "TIME");

	_time_bar->SetActionCommandWithName("PROCESS", CallFunc::create([this,call_back]() {
		this->_time_bar->ForceFinishAction();
		if (call_back)
		{
			call_back();
		}
	}));
	_time_bar->ClearHookCommand("PROCESS", "act0", 0, 2);

	dec.ReplaceDatafloat("TIME", interval / 4);
	xml::BasicDec dec1;
	dec1.InsertDatafloat("TIME", interval / 8.f);
	int i = 0;
	auto line = _time_bar->GetWidgetChildAtIdx(i);
	line->SetHookCommand("PROCESS", "act0", 0, 0, dec1, "TIME");
	line->SetActionCommandWithName("PROCESS", CallFunc::create([line, i, this , dec , dec1]() {
		line->ForceFinishAction();
		int i1 = i + 1;
		auto line1 = this->_time_bar->GetWidgetChildAtIdx(i1);
		line1->SetHookCommand("PROCESS", "act0", 0, 0, dec, "TIME");
		line1->SetActionCommandWithName("PROCESS", CallFunc::create([line1, i1, this , dec, dec1]() {
			line1->ForceFinishAction();
			int i2 = i1 + 1;
			auto line2 = this->_time_bar->GetWidgetChildAtIdx(i2);
			line2->SetHookCommand("PROCESS", "act0", 0, 0, dec, "TIME");
			line2->SetActionCommandWithName("PROCESS", CallFunc::create([line2, i2, this , dec, dec1]() {
				line2->ForceFinishAction();
				int i3 = i2 + 1;
				auto line3 = this->_time_bar->GetWidgetChildAtIdx(i3);
				line3->SetHookCommand("PROCESS", "act0", 0, 0, dec, "TIME");
				line3->SetActionCommandWithName("PROCESS", CallFunc::create([this , line3 , i3 , dec, dec1]()
				{
					line3->ForceFinishAction();
					int i4 = i3 + 1;
					auto line4 = this->_time_bar->GetWidgetChildAtIdx(i4);
					line4->SetHookCommand("PROCESS", "act0", 0, 0, dec1, "TIME");
					line4->SetActionCommandWithName("PROCESS", CallFunc::create([this, line4]()
					{
						line4->ForceFinishAction();
						for (size_t it = 0; it < this->_time_bar->GetNumberChildWidget(); it++)
						{
							auto p = this->_time_bar->GetWidgetChildAtIdx(it);
							p->SetActionCommandWithName("BEGIN_PROCESS", CallFunc::create([it, p, this] 
							{
								p->ForceFinishAction();
								if (it == _time_bar->GetNumberChildWidget() - 1)
								{
									_time_bar->ForceFinishAction();
									_time_bar->Visible(false);
								}
							}));
						}
					}));
					line4->ClearHookCommand("PROCESS", "act0", 0, 0);
				}));
				line3->ClearHookCommand("PROCESS", "act0", 0, 0);
			}));
			line2->ClearHookCommand("PROCESS", "act0", 0, 0);
		}));
		line1->ClearHookCommand("PROCESS", "act0", 0, 0);
	}));
	line->ClearHookCommand("PROCESS", "act0", 0, 0);

	_particle->SetHookCommand("MOVING", "act0", 0, 2, dec1, "TIME");
	_particle->SetHookCommand("MOVING", "act0", 0, 3, dec, "TIME");
	_particle->SetHookCommand("MOVING", "act0", 0, 4, dec, "TIME");
	_particle->SetHookCommand("MOVING", "act0", 0, 5, dec, "TIME");
	_particle->SetHookCommand("MOVING", "act0", 0, 6, dec1, "TIME");
	_particle->SetActionCommandWithName("MOVING");
	_particle->ClearHookCommand("MOVING", "act0", 0);
}
bool  RectProcessTime::isShowTimeAction()
{
	return _time_bar->Visible();
}
