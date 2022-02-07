
// chatcDlg.h: 头文件
//

#pragma once
#include "USocketClient.h"


// CchatcDlg 对话框
class CchatcDlg : public CDialogEx
{
// 构造
public:

	CchatcDlg(CWnd* pParent = nullptr);	// 标准构造函数
	USocketClient client;
	static CchatcDlg *Inst;
	void login(); //登录
	static void Onmsg(CString cs);
	User muser; //登录用户
	int sel;
	User *psel;
	void  showuser(int t=0,CString str=_T("")); //显示用户
	void addchat_11(CString cs); //私聊保存
	void addchat_1n(CString cs); //公聊保存
	void onfriend(CString cs); //添加朋友消息
	void OnFile(vector<CString>vec);
	void showbtn(); //添加好友按钮文本
	CListCtrl mlist; 
	CEdit editprichat; //私聊
	CEdit editpubchat; //公聊
	CString strtxt;
	int ncharttype;
	void showchat(CString act=_T("")); //显示聊天
	LRESULT OnMsg(WPARAM w, LPARAM l);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATC_DIALOG };
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
	afx_msg void OnDestroy();
	
	afx_msg void OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
};
