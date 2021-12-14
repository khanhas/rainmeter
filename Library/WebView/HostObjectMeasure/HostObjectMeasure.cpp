#include "StdAfx.h"
#include "HostObjectMeasure/HostObjectMeasure.h"

HostObjectMeasure::HostObjectMeasure(MeterWebView *mwv, Measure *measure) : parent(mwv), measure(measure) {}

STDMETHODIMP HostObjectMeasure::GetString(BSTR *result)
{
	const WCHAR *stringValue = measure->GetStringOrFormattedValue(AUTOSCALE_OFF, 1.0, -1, false);
	if (stringValue)
	{
		*result = SysAllocString(stringValue);
	}
	return S_OK;
}

STDMETHODIMP HostObjectMeasure::GetNumber(double *result)
{
	*result = measure->GetValue();
	return S_OK;
}

STDMETHODIMP HostObjectMeasure::GetOption(BSTR optionName, BOOL replaceVariables, BSTR *stringResult)
{
	std::wstring name(optionName);
	std::wstring value(RmReadString(measure, name.c_str(), nullptr, replaceVariables));
	*stringResult = SysAllocString(value.c_str());

	return S_OK;
}

STDMETHODIMP HostObjectMeasure::Command(BSTR command)
{
	measure->Command(std::wstring(command));
	return S_OK;
}

STDMETHODIMP HostObjectMeasure::Update()
{
	measure->Update();
	return S_OK;
}

STDMETHODIMP HostObjectMeasure::get_Disabled(BOOL *isDisabled)
{
	*isDisabled = measure->IsDisabled();
	return S_OK;
}

STDMETHODIMP HostObjectMeasure::put_Disabled(BOOL isDisabled)
{
	if (isDisabled)
	{
		measure->Disable();
	}
	else
	{
		measure->Enable();
	}
	return S_OK;
}

STDMETHODIMP HostObjectMeasure::get_Paused(BOOL *isPaused)
{
	*isPaused = measure->IsPaused();
	return S_OK;
}

STDMETHODIMP HostObjectMeasure::put_Paused(BOOL isPaused)
{
	if (isPaused)
	{
		measure->Pause();
	}
	else
	{
		measure->Unpause();
	}
	return S_OK;
}

STDMETHODIMP HostObjectMeasure::get_DynamicVariables(BOOL *hasDV)
{
	*hasDV = measure->HasDynamicVariables();
	return S_OK;
}

STDMETHODIMP HostObjectMeasure::put_DynamicVariables(BOOL hasDV)
{
	measure->SetDynamicVariables(hasDV);
	return S_OK;
}

STDMETHODIMP HostObjectMeasure::GetTypeInfoCount(UINT *pctinfo)
{
	*pctinfo = 1;
	return S_OK;
}

STDMETHODIMP HostObjectMeasure::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	if (0 != iTInfo || !parent->c_typeLib)
	{
		return TYPE_E_ELEMENTNOTFOUND;
	}

	return parent->c_typeLib->GetTypeInfoOfGuid(__uuidof(IHostObjectMeasure), ppTInfo);
}

STDMETHODIMP HostObjectMeasure::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	Microsoft::WRL::ComPtr<ITypeInfo> typeInfo;
	RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
	return typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
}

STDMETHODIMP HostObjectMeasure::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	Microsoft::WRL::ComPtr<ITypeInfo> typeInfo;
	RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
	return typeInfo->Invoke(this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}
