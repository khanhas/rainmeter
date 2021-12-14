#ifndef __METERWEBVIEW_H__
#define __METERWEBVIEW_H__

#include "StdAfx.h"
#include "WebView/webview.h"
#include "Meter.h"
#include "WebView/HostObjectRmAPI/HostObjectRmAPI.h"

class HostObjectRmAPI;
class MeterWebView : public Meter
{
public:
	MeterWebView(Skin *skin, const WCHAR *name);
	virtual ~MeterWebView();

	MeterWebView(const MeterWebView &other) = delete;
	MeterWebView &operator=(MeterWebView other) = delete;

	UINT GetTypeID() override { return TypeID<MeterWebView>(); }

	virtual void Initialize();
	virtual bool Update();
	virtual bool Draw(Gfx::Canvas &canvas);

	void CreateController();

	HRESULT CreateEnvironmentHandler(HRESULT result, ICoreWebView2Environment *env);
	HRESULT CreateControllerHandler(HRESULT result, ICoreWebView2Controller *controller);
	HRESULT MessageHandler(ICoreWebView2 *, ICoreWebView2WebMessageReceivedEventArgs *args);

	void UpdateWebViewWindow();

	static Microsoft::WRL::ComPtr<ITypeLib> c_typeLib;

protected:
	virtual void ReadOptions(ConfigParser &parser, const WCHAR *section);
	virtual void BindMeasures(ConfigParser &parser, const WCHAR *section);

private:
	Skin *skin = nullptr;
	HWND hwnd = NULL;
	std::wstring url;

	double zoomFactor = 1.0;
	Microsoft::WRL::ComPtr<ICoreWebView2Controller> controller;
	Microsoft::WRL::ComPtr<ICoreWebView2> view;
	Microsoft::WRL::ComPtr<HostObjectRmAPI> hostObject;

	static Microsoft::WRL::ComPtr<ICoreWebView2Environment> c_environment;
	static int c_InstanceCount;
};

#endif