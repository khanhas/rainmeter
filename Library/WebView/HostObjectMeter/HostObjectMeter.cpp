#include "StdAfx.h"
#include "WebView/HostObjectMeter/HostObjectMeter.h"
#include "MeterWebView.h"

HostObjectMeter::HostObjectMeter(MeterWebView *mwv, Meter *meter) : parent(mwv), meter(meter) {}

STDMETHODIMP HostObjectMeter::GetOption(BSTR optionName, BOOL replaceVariables, BSTR *stringResult)
{
	std::wstring name(optionName);
	std::wstring value(RmReadString(meter, name.c_str(), nullptr, replaceVariables));
	*stringResult = SysAllocString(value.c_str());

	return S_OK;
}

STDMETHODIMP HostObjectMeter::Update()
{
	meter->Update();
	return S_OK;
}

STDMETHODIMP HostObjectMeter::get_Hidden(BOOL *isHidden)
{
	*isHidden = meter->IsHidden();
	return S_OK;
}

STDMETHODIMP HostObjectMeter::put_Hidden(BOOL isHidden)
{
	if (isHidden)
	{
		meter->Hide();
	}
	else
	{
		meter->Show();
	}
	return S_OK;
}

STDMETHODIMP HostObjectMeter::get_DynamicVariables(BOOL *hasDV)
{
	*hasDV = meter->HasDynamicVariables();
	return S_OK;
}

STDMETHODIMP HostObjectMeter::put_DynamicVariables(BOOL hasDV)
{
	meter->SetDynamicVariables(hasDV);
	return S_OK;
}

STDMETHODIMP HostObjectMeter::get_X(int *value)
{
	*value = meter->GetX();
	return S_OK;
}

STDMETHODIMP HostObjectMeter::put_X(int value)
{
	meter->SetX(value);
	return S_OK;
}

STDMETHODIMP HostObjectMeter::get_Y(int *value)
{
	*value = meter->GetY();
	return S_OK;
}

STDMETHODIMP HostObjectMeter::put_Y(int value)
{
	meter->SetY(value);
	return S_OK;
}

STDMETHODIMP HostObjectMeter::get_Width(int *value)
{
	*value = meter->GetW();
	return S_OK;
}

STDMETHODIMP HostObjectMeter::put_Width(int value)
{
	meter->SetW(value);
	return S_OK;
}

STDMETHODIMP HostObjectMeter::get_Height(int *value)
{
	*value = meter->GetH();
	return S_OK;
}

STDMETHODIMP HostObjectMeter::put_Height(int value)
{
	meter->SetH(value);
	return S_OK;
}

STDMETHODIMP HostObjectMeter::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 1;
	return S_OK;
}

STDMETHODIMP HostObjectMeter::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	if (0 != iTInfo || !parent->c_typeLib)
	{
		return TYPE_E_ELEMENTNOTFOUND;
	}

	return parent->c_typeLib->GetTypeInfoOfGuid(__uuidof(IHostObjectMeter), ppTInfo);
}

STDMETHODIMP HostObjectMeter::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	Microsoft::WRL::ComPtr<ITypeInfo> typeInfo;
	RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
	return typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
}

STDMETHODIMP HostObjectMeter::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	Microsoft::WRL::ComPtr<ITypeInfo> typeInfo;
	RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
	return typeInfo->Invoke(this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}
