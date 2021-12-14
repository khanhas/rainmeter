#include "StdAfx.h"
#include "Logger.h"
#include "HostObjectRmAPI.h"

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
