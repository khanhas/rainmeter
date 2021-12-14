#ifndef __WEBVIEW_H__
#define __WEBVIEW_H__

#include <Windows.h>

#include "StdAfx.h"
#include "../Plugins/API/RainmeterAPI.h"

#include <wrl.h>
#include <wrl/client.h>
#include <map>
#include <string>
#include <vector>
#include <functional>
#include <WebView2.h>

#include "MeterWebView.h"

#define RETURN_IF_FAILED(hr) if (FAILED(hr)) { return hr; }
#endif __WEBVIEW_H__
