#ifndef __WEBVIEW_HOSTOBJECT_RMAPI_H__
#define __WEBVIEW_HOSTOBJECT_RMAPI_H__

#include "HostObject_h.h"
#include "HostObjectMeasure/HostObjectMeasure.h"
#include "HostObjectMeter/HostObjectMeter.h"

class MeterWebView;

class HostObjectRmAPI : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IHostObjectRmAPI, IDispatch>
{
public:
	HostObjectRmAPI(MeterWebView *mwv);

	// Methods
	STDMETHODIMP GetMeasure(BSTR name, HostObjectMeasure **measureObj) override;
	STDMETHODIMP GetMeter(BSTR name, HostObjectMeter **meterObj) override;
	STDMETHODIMP GetVariable(BSTR stringParameter, BSTR *stringResult) override;
	STDMETHODIMP Bang(BSTR stringParameter) override;
	STDMETHODIMP Log(BSTR stringParameter) override;
	STDMETHODIMP ForwardEvent(BSTR eventType, int which, int x, int y) override;

	// Properties

	// Host Object essentials
	STDMETHODIMP GetTypeInfoCount(UINT *pctinfo) override;
	STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) override;
	STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) override;
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) override;

private:
	MeterWebView *parent;
	Skin *skin;
};

#endif __WEBVIEW_HOSTOBJECT_RMAPI_H__
