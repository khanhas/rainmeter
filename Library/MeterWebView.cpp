#include "StdAfx.h"
#include "MeterWebView.h"
#include "Logger.h"
#include "Rainmeter.h"
#include "json/json.hpp"

#define WEBVIEW_LAYER_CLASS_NAME L"RainmeterWebViewLayer"

using namespace Microsoft::WRL;
using nlohmann::json;

int MeterWebView::c_InstanceCount = 0;
ComPtr<ITypeLib> MeterWebView::c_typeLib = nullptr;
ComPtr<ICoreWebView2Environment> MeterWebView::c_environment = nullptr;

MeterWebView::MeterWebView(Skin *skin, const WCHAR *name) : Meter(skin, name)
{
	Meter::Initialize();

	LPWSTR versionInfo = nullptr;
	GetAvailableCoreWebView2BrowserVersionString(nullptr, &versionInfo);
	if (versionInfo == nullptr)
	{
		auto command = MessageBox(nullptr, L"Click OK to get one from Microsoft", L"WebView2 runtime not found", MB_OKCANCEL | MB_ICONWARNING);

		if (command == IDOK)
		{
			ShellExecute(0, 0, L"https://go.microsoft.com/fwlink/p/?LinkId=2124703", 0, 0, SW_SHOW);
		}
		return;
	}

	if (c_InstanceCount == 0)
	{
		WNDCLASSEX wc = {sizeof(WNDCLASSEX)};
		wc.style = CS_NOCLOSE | CS_DBLCLKS;
		wc.lpfnWndProc = DefWindowProc;
		wc.hInstance = GetRainmeter().GetModuleInstance();
		wc.hCursor = nullptr;
		wc.lpszClassName = WEBVIEW_LAYER_CLASS_NAME;
		RegisterClassEx(&wc);

		// TODO: Embed and load this file
		LoadTypeLib(L"Library.tlb", &c_typeLib);
	}

	++c_InstanceCount;
}

MeterWebView::~MeterWebView()
{
	view->Stop();
	controller->Close();

	if (hwnd)
	{
		DestroyWindow(hwnd);
		hwnd = NULL;
	}

	--c_InstanceCount;
	if (c_InstanceCount == 0)
	{
		UnregisterClass(WEBVIEW_LAYER_CLASS_NAME, GetRainmeter().GetModuleInstance());
		c_typeLib = nullptr;
		c_environment = nullptr;
	}
}

void MeterWebView::Initialize()
{
	Meter::Initialize();

	wchar_t userFolder[MAX_PATH];
	GetTempPath(MAX_PATH, userFolder);
	wcscat(userFolder, L"RmWebViewUserFolder");

	// Initiate WebView2
	if (c_environment == nullptr)
	{
		CreateCoreWebView2EnvironmentWithOptions(nullptr, userFolder, nullptr,
												 Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this, &MeterWebView::CreateEnvironmentHandler).Get());
	}
	else
	{
		CreateController();
	}
}

void MeterWebView::ReadOptions(ConfigParser &parser, const WCHAR *section)
{
	Meter::ReadOptions(parser, section);

	url = parser.ReadString(section, L"URL", L"");
	zoomFactor = parser.ReadFloat(section, L"Zoom", 1.0);
}

void MeterWebView::BindMeasures(ConfigParser &parser, const WCHAR *section)
{
	if (BindPrimaryMeasure(parser, section, true))
	{
		BindSecondaryMeasures(parser, section);
	}
}

bool MeterWebView::Draw(Gfx::Canvas &canvas)
{
	UpdateWebViewWindow();
	return Meter::Draw(canvas);
}

bool MeterWebView::Update() { return false; }

void MeterWebView::CreateController()
{
	hwnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_LAYERED, WEBVIEW_LAYER_CLASS_NAME, nullptr, WS_POPUP, m_X, m_Y, m_W, m_H, nullptr, nullptr, GetRainmeter().GetModuleInstance(), nullptr);

	ShowWindow(hwnd, SW_SHOWNORMAL);
	SetParent(hwnd, GetSkin()->GetWindow());

	c_environment->CreateCoreWebView2Controller(hwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(this, &MeterWebView::CreateControllerHandler).Get());
}

HRESULT MeterWebView::CreateEnvironmentHandler(HRESULT result, ICoreWebView2Environment *env)
{
	c_environment = env;
	CreateController();

	return S_OK;
}

HRESULT MeterWebView::CreateControllerHandler(HRESULT result, ICoreWebView2Controller *ctl)
{
	if (ctl != nullptr)
	{
		controller = ctl;
		controller->get_CoreWebView2(&view);
	}
	else
	{
		return S_FALSE;
	}

	// Settings for the webview
	ICoreWebView2Settings *settings;
	view->get_Settings(&settings);
	settings->put_IsScriptEnabled(TRUE);
	settings->put_AreDefaultScriptDialogsEnabled(TRUE);
	settings->put_IsWebMessageEnabled(TRUE);
	settings->put_AreHostObjectsAllowed(TRUE);

	UpdateWebViewWindow();

	// Transparent background
	ComPtr<ICoreWebView2Controller2> controller2;
	controller.As<ICoreWebView2Controller2>(&controller2);
	COREWEBVIEW2_COLOR m_webViewColor = {0, 0, 0, 0};
	controller2->put_DefaultBackgroundColor(m_webViewColor);

	if (wcsncmp(url.c_str(), L"http", 4) != 0 && wcsncmp(url.c_str(), L"file://", 7) != 0)
	{
		GetSkin()->MakePathAbsolute(url);
		url.insert(0, L"file://");
	}

	view->Navigate(url.c_str());
	// view->OpenDevToolsWindow();

	hostObject = Make<HostObjectRmAPI>(this);

	// Add Host Object exposing Rainmeter API to environment
	VARIANT variant = {};
	ComPtr<IDispatch> dispHO;
	hostObject.CopyTo<IDispatch>(&dispHO);
	variant.pdispVal = dispHO.Get();
	variant.vt = VT_DISPATCH;
	view->AddHostObjectToScript(L"RainmeterAPI", &variant);

	// Script to add global methods
	view->AddScriptToExecuteOnDocumentCreated(
		LR"(
RainmeterAPI = chrome.webview.hostObjects.sync.RainmeterAPI;
window.addEventListener('DOMContentLoaded', () => {
	const downAndMoveHandler = () => RainmeterAPI.forwardEvent("drag", 1, 0, 0);

	let lastDownEvent = {};
	document.addEventListener("mousedown", (event) => {
		document.addEventListener("mousemove", downAndMoveHandler);
		if (event.which == 1 && event.target.tagName == "BUTTON") return;

		const { type, which, screenX, screenY } = event;
		const timestamp = (new Date()).getTime();


		if (which == lastDownEvent.which &&
			(timestamp - lastDownEvent.timestamp) < 500) {
			RainmeterAPI.forwardEvent("dblclick", which, screenX, screenY);
			return;
		}

		lastDownEvent = { which, timestamp };

		RainmeterAPI.forwardEvent(type, which, screenX, screenY);
	});

	document.addEventListener("mouseup", (event) => {
		document.removeEventListener("mousemove", downAndMoveHandler);
		if (event.which == 1 && event.target.tagName == "BUTTON") return;

		const modifiers = event.ctrlKey | event.altKey << 1 | event.shiftKey << 2;
		RainmeterAPI.forwardEvent(event.type, event.which, event.screenX, event.screenY, modifiers);
	});

	document.addEventListener("contextmenu", (event) => {
		if (event.shiftKey) return; // Using default WebView2 context menu
		
		event.preventDefault();
	});

	document.addEventListener("mouseenter", (event) => RainmeterAPI.forwardEvent(event.type, 0, 0, 0));
	document.addEventListener("mouseleave", (event) => RainmeterAPI.forwardEvent(event.type, 0, 0, 0));
	document.addEventListener("mousewheel", (event) => RainmeterAPI.forwardEvent(event.type, 0, event.deltaX, event.deltaY));
});
		)",
		nullptr);

	EventRegistrationToken token;
	view->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>(this, &MeterWebView::MessageHandler).Get(), &token);

	return S_OK;
}

HRESULT MeterWebView::MessageHandler(ICoreWebView2 *, ICoreWebView2WebMessageReceivedEventArgs *args)
{
	PWSTR message;
	args->get_WebMessageAsJson(&message);
	std::wstring msg(message);
	CoTaskMemFree(message);

	// Parse mouse position
	json status = json::parse(msg, nullptr, false);
	if (status.is_null() || status.is_discarded())
	{
		return S_FALSE;
	}

	if (status["type"] == "contextmenu")
	{
		long x = status["x"].get<long>();
		long y = status["y"].get<long>();
		GetRainmeter().ShowContextMenu(POINT{x, y}, GetSkin());
	}

	return S_OK;
}

void MeterWebView::UpdateWebViewWindow()
{
	if (controller == nullptr)
	{
		return;
	}

	SetWindowPos(hwnd, nullptr, m_X, m_Y, m_W, m_H, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);

	RECT bounds;
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = m_W;
	bounds.bottom = m_H;

	controller->SetBoundsAndZoomFactor(bounds, zoomFactor);

	if (m_Hidden)
	{
		ShowWindow(hwnd, SW_HIDE);
		controller->put_IsVisible(false);
	}
	else
	{
		ShowWindow(hwnd, SW_SHOWNOACTIVATE);
		controller->put_IsVisible(true);
	}
}
