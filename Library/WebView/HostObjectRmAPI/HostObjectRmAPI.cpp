#include "StdAfx.h"
#include "Logger.h"
#include "HostObjectRmAPI.h"
#include "Rainmeter.h"

HostObjectRmAPI::HostObjectRmAPI(MeterWebView *mwv) : parent(mwv), skin(parent->GetSkin()) {}

STDMETHODIMP HostObjectRmAPI::GetMeasure(BSTR name, HostObjectMeasure **measureObj)
{
	Measure *target = nullptr;
	for (auto m : skin->GetMeasures())
	{
		if (_wcsicmp(name, m->GetName()) == 0)
		{
			target = m;
			break;
		}
	}
	if (target == nullptr)
	{
		measureObj = nullptr;
		return S_FALSE;
	}

	auto host = Microsoft::WRL::Make<HostObjectMeasure>(parent, target);
	host.CopyTo(measureObj);
	return S_OK;
}

STDMETHODIMP HostObjectRmAPI::GetMeter(BSTR name, HostObjectMeter **meterObj)
{
	Meter *target = nullptr;
	for (auto m : skin->GetMeters())
	{
		if (_wcsicmp(name, m->GetName()) == 0)
		{
			target = m;
			break;
		}
	}
	if (target == nullptr)
	{
		meterObj = nullptr;
		return S_FALSE;
	}

	auto host = Microsoft::WRL::Make<HostObjectMeter>(parent, target);
	host.CopyTo(meterObj);
	return S_OK;
}

STDMETHODIMP HostObjectRmAPI::GetVariable(BSTR stringParameter, BSTR *stringResult)
{
	std::wstring name(stringParameter);
	auto value = skin->GetParser().GetVariable(name);
	*stringResult = SysAllocString(value->c_str());

	return S_OK;
}

STDMETHODIMP HostObjectRmAPI::Bang(BSTR stringParameter)
{
	std::wstring bang(stringParameter);
	RmExecute(parent->GetSkin(), stringParameter);
	return S_OK;
}

STDMETHODIMP HostObjectRmAPI::Log(BSTR stringParameter)
{
	std::wstring message(stringParameter);
	RmLog(parent, (int)Logger::Level::Notice, message.c_str());

	return S_OK;
}

STDMETHODIMP HostObjectRmAPI::ForwardEvent(BSTR eventType, int which, int x, int y, int modifiers = 0)
{
	constexpr auto LEFT_MOUSE = 1;
	constexpr auto MIDDLE_MOUSE = 2;
	constexpr auto RIGHT_MOUSE = 3;
	constexpr auto X1_MOUSE = 4;
	constexpr auto X2_MOUSE = 5;
	#define EXECUTE_ACTION(type) { auto &cmd = parent->GetMouse().Get##type##Action(); if (!cmd.empty()) { GetRainmeter().ExecuteActionCommand(cmd.c_str(), parent); } }

	auto hwnd = skin->GetWindow();
	std::wstring type(eventType);
	LPARAM lParam = MAKELPARAM(x - skin->GetX() + parent->GetX(), y - skin->GetY() + parent->GetY());
	WPARAM wParam = 0;

	bool ctrlKey = (modifiers & 0x01) != 0;
	bool altKey = (modifiers & 0x02) != 0;
	bool shiftKey = (modifiers & 0x04) != 0;

	if (type == L"drag")
	{
		ReleaseCapture();
		SendMessage(hwnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, lParam);
	}
	else if (type == L"mouseup")
	{
		switch (which)
		{
			case LEFT_MOUSE: EXECUTE_ACTION(LeftUp); break;
			case MIDDLE_MOUSE: EXECUTE_ACTION(MiddleUp); break;
			case RIGHT_MOUSE:
				if (!shiftKey)
				{
					SendMessage(hwnd, WM_RBUTTONUP, wParam, lParam);
				}
				break;
			case X1_MOUSE: EXECUTE_ACTION(X1Up); break;
			case X2_MOUSE: EXECUTE_ACTION(X2Up); break;
		}
	}
	else if (type == L"mousedown")
	{
		switch (which)
		{
			case LEFT_MOUSE: EXECUTE_ACTION(LeftDown); break;
			case MIDDLE_MOUSE: EXECUTE_ACTION(MiddleDown); break;
			case RIGHT_MOUSE: EXECUTE_ACTION(RightDown); break;
			case X1_MOUSE: EXECUTE_ACTION(X1Down); break;
			case X2_MOUSE: EXECUTE_ACTION(X2Down); break;
		}
	}
	else if (type == L"dblclick")
	{
		switch (which)
		{
			case LEFT_MOUSE: EXECUTE_ACTION(LeftDoubleClick); break;
			case MIDDLE_MOUSE: EXECUTE_ACTION(MiddleDoubleClick); break;
			case RIGHT_MOUSE: EXECUTE_ACTION(RightDoubleClick); break;
			case X1_MOUSE: EXECUTE_ACTION(X1DoubleClick); break;
			case X2_MOUSE: EXECUTE_ACTION(X2DoubleClick); break;
		}
	}
	else if (type == L"mouseenter")
	{
		EXECUTE_ACTION(Over)
	}
	else if (type == L"mouseleave")
	{
		EXECUTE_ACTION(Leave)
	}
	else if (type == L"mousewheel")
	{
		if (y < 0) EXECUTE_ACTION(MouseScrollUp)
		else if (y > 0) EXECUTE_ACTION(MouseScrollDown)
		else if (x < 0) EXECUTE_ACTION(MouseScrollLeft)
		else if (x > 0) EXECUTE_ACTION(MouseScrollRight)
	}

	return S_OK;
}

STDMETHODIMP HostObjectRmAPI::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 1;
	return S_OK;
}

STDMETHODIMP HostObjectRmAPI::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	if (0 != iTInfo || !parent->c_typeLib)
	{
		return TYPE_E_ELEMENTNOTFOUND;
	}
	return parent->c_typeLib->GetTypeInfoOfGuid(__uuidof(IHostObjectRmAPI), ppTInfo);
}

STDMETHODIMP HostObjectRmAPI::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	Microsoft::WRL::ComPtr<ITypeInfo> typeInfo;
	RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
	return typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
}

STDMETHODIMP HostObjectRmAPI::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	Microsoft::WRL::ComPtr<ITypeInfo> typeInfo;
	RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
	return typeInfo->Invoke(this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}
