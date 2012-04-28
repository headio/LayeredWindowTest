// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "BkgWindow.h"

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	/*HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);*/

  HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
    IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
  SetIcon(hIcon, TRUE);
  HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
    IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
  SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);


  bkgwnd.Create(NULL);
  bkgwnd.ShowWindow(SW_SHOW);


  InitMainWindow();

	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

  bkgwnd.DestroyWindow();

	return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	CloseDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

void PreMultiply( CImage& image )
{
  int width = image.GetWidth();
  int height = image.GetHeight();
  byte* p;
  byte alpha;
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      p = (byte*)image.GetPixelAddress(col, row);
      alpha = p[3];
      if (255 > alpha) {
        p[0] = ((p[0] * alpha) + 127) / 255;
        p[1] = ((p[1] * alpha) + 127) / 255;
        p[2] = ((p[2] * alpha) + 127) / 255;
      }
    }
  }
}


void CMainDlg::InitMainWindow( void )
{
  //设置为层窗口
  LONG wndLong = GetWindowLong(GWL_EXSTYLE);
  wndLong |= WS_EX_LAYERED;
  SetWindowLong(GWL_EXSTYLE, wndLong);
  HDC hWndDC = GetDC();
  HDC hMemDC = CreateCompatibleDC(hWndDC);
  CImage image;
  //载入一幅自己的图片
  image.Load(L"res/bkg.png");
  //将图片预乘
  PreMultiply(image);
  //创建兼容Bitmap，并将其select进去
  HBITMAP hMemBmp = CreateCompatibleBitmap(hWndDC, image.GetWidth(), image.GetHeight());
  SelectObject(hMemDC, hMemBmp);
  //画入到DC
  image.Draw(hMemDC, 0, 0, image.GetWidth(), image.GetHeight());
  CRect rc;
  GetWindowRect(&rc);
  CPoint leftTop(rc.TopLeft());
  CPoint ptDC(0, 0);
  CSize wndSize(image.GetWidth(), image.GetHeight());
  BLENDFUNCTION bf;
  bf.BlendOp = AC_SRC_OVER;
  bf.AlphaFormat = AC_SRC_ALPHA;
  bf.SourceConstantAlpha = 0xff;
  bf.BlendFlags = 0;
  bool bRet = UpdateLayeredWindow(m_hWnd, NULL, &leftTop, &wndSize, hMemDC, &ptDC,
    0, &bf, ULW_ALPHA);

}
