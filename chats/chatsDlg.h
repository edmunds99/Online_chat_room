
// chatsDlg.h: 头文件
//

#pragma once

#include "USocketServer.h"
// CchatsDlg 对话框
class CchatsDlg : public CDialogEx
{
// 构造
public:
	CchatsDlg(CWnd* pParent = nullptr);	// 标准构造函数
	USocketServer svr;
	LRESULT onmessage(WPARAM w, LPARAM l);
	void showdb();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CEdit info;
	CString port;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton3();
};
