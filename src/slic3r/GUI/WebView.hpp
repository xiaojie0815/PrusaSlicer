#ifndef slic3r_GUI_WebView_hpp_
#define slic3r_GUI_WebView_hpp_

#include <vector>
#include <string>

class wxWebView;
class wxWindow;
class wxString;

namespace WebView
{
    // When using WebView in Panel, it is benfitable to call create only when it is being shown. (lower CPU usage)
    // But when using WebView in Dialog, call both.
    wxWebView* webview_new();
    void       webview_create(wxWebView* webview, wxWindow *parent, const wxString& url, const std::vector<std::string>& message_handlers);     
};

#endif // !slic3r_GUI_WebView_hpp_
