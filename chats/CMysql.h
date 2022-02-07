#pragma once
#ifndef _CMysql
#define _CMysql
#include "pch.h"
#include "mysql.h"
#include "User.h"
#define SUCC _T("succ")
#define USER  _T("create table if not exists cuser\
(\
	uid int unsigned not null auto_increment comment '用户',\
	act varchar(30) not null comment '用户名',\
	pwd varchar(50) comment '密码',\
	code varchar(30) not null comment '身份证',\
	phone varchar(30) not null comment '手机',\
	primary key(uid)\
	);")
#define FRIEND  _T("create table if not exists friend\
(\
	uid1 int unsigned not null comment '用户',\
	uid2 int unsigned not null comment '用户朋友',\
	time datetime not null comment '添加时间',\
	foreign key(uid1) references cuser(uid) on delete cascade on update cascade,\
    foreign key(uid2) references cuser(uid) on delete cascade on update cascade,\
	primary key(uid1,uid2)\
	);")






class CMysql
{
public:
	static bool binit;		//是否初始化
	static CString mdir;	 //程序目录
	MYSQL *pcon;			 //连接
	CString mhost;			 //主机
	CString muser;			 //用户
	CString mpwd;			//密码
	CString mdb;			//数据库
	CString charset;		//字符集
	short port;            //端口
	bool mck;				//是否新建数据库
	int state;
	CMysql();
	CMysql(CString cs);

	~CMysql()
	{
		close();
	}
	static string Utf8ToAscii(const string& str);   //转码
	static string AsciiToUtf8(const string& str);
	static wstring AsciiToUnicode(const string& str);   
	static string UnicodeToAscii(const wstring& wstr);
	static string UnicodeToUtf8(const wstring& wstr);
	static wstring Utf8ToUnicode(const string& str);
	BOOL init();				  //初始化库和连接
	void close(bool bl = false);  //退出
	void readini();					//数据库配置读取
	void saveini(CString h, int p, CString u, CString w, CString d);
	CString usedb(CString db);		//使用数据库
	CString setcharset(CString cs);	 //设置字符集
	bool bconnect();		 //是否登录
	CString geterr();		//错误信息
	CString excutecmd(CString cs, vector<vector<CString>> &vec);  //查询
	CString excutecmd(CString sql);	 //查询
	CString login(CString h, int port, CString u, CString p, CString d, bool bl);  //登录
	CString login(bool bl);  //登录
	CString createtatle();		//创建表
	CString createdatabase();	//创建数据库

	//按用户名查找用户
	CString finduser(CString n, vector<vector<CString>>&res);
	CString reg(vector<CString>&vec);//注册
	//登录vec为用户数据
	CString login(vector<CString>&vec,vector<vector<CString>>&res);
	CString getfriends(vector<Friend> &records); //二进制数据查询
	CString getusers(vector<vector<CString>>&res);
	CString addfriend(int id1,int id2);//注册
	CString delfriend(int id1, int id2);//注册
};

#endif

