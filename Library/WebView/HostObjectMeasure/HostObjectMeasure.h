#ifndef __WEBVIEW_HOSTOBJECT_MEASURE_H__
#define __WEBVIEW_HOSTOBJECT_MEASURE_H__

#include "HostObject_h.h"
#include "webview.h"
#include "Measure.h"

class MeterWebView;

class HostObjectMeasure : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IHostObjectMeasure, IDispatch>
{
public:
	HostObjectMeasure(MeterWebView *mwv, Measure *measure);

	// Methods
	STDMETHODIMP GetString(BSTR *result) override;
	STDMETHODIMP GetNumber(double *result) override;
	STDMETHODIMP GetOption(BSTR optionName, BOOL replaceVariables, BSTR *stringResult) override;
	STDMETHODIMP Command(BSTR command) override;
	STDMETHODIMP Update() override;

	// Properties
	STDMETHODIMP get_Disabled(BOOL *isDisabled) override;
	STDMETHODIMP put_Disabled(BOOL isDisabled) override;
	STDMETHODIMP get_Paused(BOOL *isPaused) override;
	STDMETHODIMP put_Paused(BOOL isPaused) override;
	STDMETHODIMP get_DynamicVariables(BOOL *hasDV) override;
	STDMETHODIMP put_DynamicVariables(BOOL hasDV) override;

	// Host Object essentials
	STDMETHODIMP GetTypeInfoCount(UINT *pctinfo) override;
	STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) override;
	STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) override;
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) override;

private:
	MeterWebView *parent;
	Measure *measure;
};

#endif __WEBVIEW_HOSTOBJECT_MEASURE_H__