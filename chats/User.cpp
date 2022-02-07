#include "pch.h"
#include "User.h"
vector<Friend>Friend::friends;
vector<User>User::users;
vector<CChat>CChat::chats;
CString CChat::lpath=_T("");
void CChat::readchat(int id)
{
	CString file = getpath();
	CString str;
	str.Format(_T("%d_%s"), id,CHATFILE);


	file += str;
	CString cs;
	readfile(file, cs);
	vector<CString>line;
	User::readwords(cs, '\n', line);
	for (int i = 0;i < line.size();i++)
	{
		CChat::addchat(line[i]);
	}
}

void User::clear()
{

	users.clear();
}

void User::getuser(CString cs) //读取用户
{
	clear();
	
	vector<CString>vec;
	vector<CString>line;
	readwords(cs, '\n', vec);
	User u;
	for (int i = 0;i < vec.size();i++)
	{
		readwords(vec[i], ',', line);
		u = getuser(line);
		if (u.uid!= -1)
		{
			users.push_back(u);
		}
	}
}

void User::getuser(vector<vector<CString>>&vec) //读取用户
{
	clear();
	for (int i = 0;i < vec.size();i++)
	{
		User u = getuser(vec[i]);
		if (u .uid!=-1)
		{
			users.push_back(u);
		}
	}
}

User User::getuser(vector<CString>&vec) //读取用户
{
	User u;
	if (vec.size() >= 5)
	{
		u.uid = _wtoi(vec[0]);
		u.act = vec[1];
		u.pwd = vec[2];
		u.code = vec[3];
		u.phone = vec[4];
	}
	return u;
}
int User::find(int id) //读取用户
{
	for (int i = 0;i < users.size();i++)
	{
		if (users[i].uid == id)
		{
			return i;
		}
	}
	return -1;
}

int User::find(CString cs) //读取用户
{
	for (int i = 0;i < users.size();i++)
	{
		if (users[i].act == cs)
		{
			return i;
		}
	}
	return -1;
}

void User::readwords(CString cs, char c, vector<CString>&vec,int n)
{
	vec.clear();
	cs = cs.Trim();
	if (cs.IsEmpty())
	{
		return;
	}
	int len = cs.GetLength();
	bool badd = false;
	if (cs[len - 1] != c)
	{
		badd = true;
		cs += c;
	}
	int pos1 = 0;
	int pos2 = cs.Find(c, pos1); //查找c位置
	CString str;
	while (pos2 != -1)
	{
		str = cs.Mid(pos1, pos2 - pos1);	//提取子字符串
		str = str.Trim();
		if (n >1)
		{
			if (vec.size()<n-1)
			{
				vec.push_back(str);
			}
			if (vec.size() == n - 1)
			{
				str = cs.Mid(pos2+1);
				if (badd)
				{
					int len = str.GetLength();
					if (len > 0)
					{
						str = str.Left(len - 1);
					}
				}
				vec.push_back(str);
				return;
			}
		}
		else
		{
			
			vec.push_back(str);
		}
		
		
		pos1 = pos2 + 1;
		pos2 = cs.Find(c, pos1); //查找c位置
	}
}

int User::findfriend(CString n)
{

	for (int i = 0;i < friends.size();i++)
	{
		if (friends[i].act == n)
		{
			return i;
		}
	}
	return -1;
}

void User::delfriend(CString n)
{
	for (vector<User>::iterator it = friends.begin();it != friends.end();it++)
	{
	
		if (it->act == n)
		{
			friends.erase(it);
			break;
		}
	}
}

void User::setfriends(vector<Friend>arr)
{
	
	int id;
	int nid;
	for (int i = 0;i < arr.size();i++)
	{
		if (arr[i].uid1 == uid)
		{
			nid = User::find(arr[i].uid2);
			if (nid != -1)
			{
				id = findfriend(User::users[nid].act);
				if (id ==-1)
				{
					friends.push_back(User::users[nid]);
				}
				
			}
		}
		else if (arr[i].uid2 == uid)
		{
			nid = User::find(arr[i].uid1);
			if (nid != -1)
			{
				id = findfriend(User::users[nid].act);
				if (id == -1)
				{
					friends.push_back(User::users[nid]);
				}

			}
		}
	}
}

void Friend::clear()
{

	friends.clear();
}
void Friend::getfriend(CString cs) //读取用户
{
	clear();

	vector<CString>vec;
	vector<CString>line;
	User::readwords(cs, '\n', vec);
	Friend u;
	for (int i = 0;i < vec.size();i++)
	{
		User::readwords(vec[i], ',', line);
		u = getfriend(line);
		if (u.uid1!=-1)
		{
			friends.push_back(u);
		}
	}
}

void Friend::getfriend(vector<vector<CString>>&vec) //读取用户
{
	clear();
	Friend u;
	for (int i = 0;i < vec.size();i++)
	{
		u = getfriend(vec[i]);
		if (u.uid1 !=-1)
		{
			friends.push_back(u);
		}
	}
}
Friend Friend::readfriend(vector<CString>&vec)
{
	Friend tmp;
	if (vec.size() >= 3)
	{
		tmp.uid1 = _wtoi(vec[0]);
		tmp.uid2 = _wtoi(vec[1]);
		tmp.strtime = vec[2];
	}
	return tmp;
}
Friend Friend::getfriend(vector<CString>&vec) //读取用户
{
	Friend tmp;
	if (vec.size() >= 3)
	{
		tmp.uid1 = _wtoi(vec[0]);
		tmp.uid2 = _wtoi(vec[1]);
		tmp.strtime = vec[2];
	}
	return tmp;
}

int Friend::find(int id1,int id2) //读取用户
{
	for (int i = 0;i < friends.size();i++)
	{
		if (friends[i].uid1 == id1 && friends[i].uid2 == id2)
		{
			return i;
		}
		else if (friends[i].uid1 == id2 && friends[i].uid2 == id1)
		{
			return i;
		}
	}
	return -1;
}