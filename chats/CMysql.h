#pragma once
#ifndef _CMysql
#define _CMysql
#include "pch.h"
#include "mysql.h"
#include "User.h"
#define SUCC _T("succ")
#define USER  _T("create table if not exists cuser\
(\
	uid int unsigned not null auto_increment comment '�û�',\
	act varchar(30) not null comment '�û���',\
	pwd varchar(50) comment '����',\
	code varchar(30) not null comment '���֤',\
	phone varchar(30) not null comment '�ֻ�',\
	primary key(uid)\
	);")
#define FRIEND  _T("create table if not exists friend\
(\
	uid1 int unsigned not null comment '�û�',\
	uid2 int unsigned not null comment '�û�����',\
	time datetime not null comment '���ʱ��',\
	foreign key(uid1) references cuser(uid) on delete cascade on update cascade,\
    foreign key(uid2) references cuser(uid) on delete cascade on update cascade,\
	primary key(uid1,uid2)\
	);")






class CMysql
{
public:
	static bool binit;		//�Ƿ��ʼ��
	static CString mdir;	 //����Ŀ¼
	MYSQL *pcon;			 //����
	CString mhost;			 //����
	CString muser;			 //�û�
	CString mpwd;			//����
	CString mdb;			//���ݿ�
	CString charset;		//�ַ���
	short port;            //�˿�
	bool mck;				//�Ƿ��½����ݿ�
	int state;
	CMysql();
	CMysql(CString cs);

	~CMysql()
	{
		close();
	}
	static string Utf8ToAscii(const string& str);   //ת��
	static string AsciiToUtf8(const string& str);
	static wstring AsciiToUnicode(const string& str);   
	static string UnicodeToAscii(const wstring& wstr);
	static string UnicodeToUtf8(const wstring& wstr);
	static wstring Utf8ToUnicode(const string& str);
	BOOL init();				  //��ʼ���������
	void close(bool bl = false);  //�˳�
	void readini();					//���ݿ����ö�ȡ
	void saveini(CString h, int p, CString u, CString w, CString d);
	CString usedb(CString db);		//ʹ�����ݿ�
	CString setcharset(CString cs);	 //�����ַ���
	bool bconnect();		 //�Ƿ��¼
	CString geterr();		//������Ϣ
	CString excutecmd(CString cs, vector<vector<CString>> &vec);  //��ѯ
	CString excutecmd(CString sql);	 //��ѯ
	CString login(CString h, int port, CString u, CString p, CString d, bool bl);  //��¼
	CString login(bool bl);  //��¼
	CString createtatle();		//������
	CString createdatabase();	//�������ݿ�

	//���û��������û�
	CString finduser(CString n, vector<vector<CString>>&res);
	CString reg(vector<CString>&vec);//ע��
	//��¼vecΪ�û�����
	CString login(vector<CString>&vec,vector<vector<CString>>&res);
	CString getfriends(vector<Friend> &records); //���������ݲ�ѯ
	CString getusers(vector<vector<CString>>&res);
	CString addfriend(int id1,int id2);//ע��
	CString delfriend(int id1, int id2);//ע��
};

#endif

