#pragma once


// fileProDlg 对话框
#include "USocketClient.h"
class fileProDlg : public CDialogEx
{
	DECLARE_DYNAMIC(fileProDlg)

public:
	fileProDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~fileProDlg();
	UFile *mfile;
	bool brec;
	void update();
	LRESULT OnMsg(WPARAM w, LPARAM l);
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl mpro;
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CString mstr;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCancel2();
};
