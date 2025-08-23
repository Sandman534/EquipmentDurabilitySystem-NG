#pragma once

#include <chrono>

class DurabilityMenu : public RE::IMenu,
	public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
	public RE::BSTEventSink<SKSE::ActionEvent>
{

private:
	using EventResult = RE::BSEventNotifyControl;
	using UI_MESSAGE_RESULTS = RE::UI_MESSAGE_RESULTS;
	using chrono_clock = std::chrono::steady_clock;

protected:
	static DurabilityMenu * ms_pSingleton;

public:
	// Constructor
	DurabilityMenu();
	virtual ~DurabilityMenu();

	// New Methods
	static bool Register();
	static DurabilityMenu * GetSingleton() { return ms_pSingleton; }
	void hotkeyActivation();

	// Override IMenu
	virtual UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override;
	virtual void AdvanceMovie(float a_interval, std::uint32_t a_currentTime) override;

	// Override BSTEventSink
	virtual EventResult ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;
	virtual EventResult ProcessEvent(const SKSE::ActionEvent* a_event, RE::BSTEventSource<SKSE::ActionEvent>* a_eventSource) override;
	
protected:
	void OnMenuOpen();
	void OnMenuClose();
	void UpdateMenu();

	int updateCount;
	chrono_clock::time_point _current;
	chrono_clock::time_point _logged;

	bool hotkeyActivated;
	bool skseActivated;

	bool visible;
};