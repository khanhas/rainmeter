#ifndef __WEBVIEW_HOSTOBJECT_METER_H__
#define __WEBVIEW_HOSTOBJECT_METER_H__

#include "HostObject_h.h"
#include "webview.h"
#include "Meter.h"

class MeterWebView;

class HostObjectMeter : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IHostObjectMeter, IDispatch>
{
public:
	HostObjectMeter(MeterWebView *mwv, Meter *meter);

	// Methods
	STDMETHODIMP GetOption(BSTR optionName, BOOL replaceVariables, BSTR *stringResult) override;
	STDMETHODIMP Update() override;

	// Properties
	STDMETHODIMP get_Hidden(BOOL *isHidden) override;
	STDMETHODIMP put_Hidden(BOOL isHidden) override;
	STDMETHODIMP get_DynamicVariables(BOOL *hasDV) override;
	STDMETHODIMP put_DynamicVariables(BOOL hasDV) override;
	STDMETHODIMP get_X(int *value) override;
	STDMETHODIMP put_X(int value) override;
	STDMETHODIMP get_Y(int *value) override;
	STDMETHODIMP put_Y(int value) override;
	STDMETHODIMP get_Width(int *value) override;
	STDMETHODIMP put_Width(int value) override;
	STDMETHODIMP get_Height(int *value) override;
	STDMETHODIMP put_Height(int value) override;

	// Host Object essentials
	STDMETHODIMP GetTypeInfoCount(UINT *pctinfo) override;
	STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) override;
	STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) override;
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) override;

private:
	MeterWebView *parent;
	Meter *meter;
};

#endif __WEBVIEW_HOSTOBJECT_METER_H__