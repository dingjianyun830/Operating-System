/* A simple server in the internet domain using TCP
The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <ctime>

using namespace std;

struct USER
{
	string usr = "";
	string pwd = "";
};

struct MESSAGE
{
	int m_nUID = -1;
	int timestamp = -1;
	bool isRead = false;
	string m_nSender = "";
	string m_nReceiver = "";
	string mg = "";
};

// Current Client
string UserName = "";
string Password = "";

// vector for Users
vector<USER> UserList;
// vector for all the message
vector<MESSAGE> MGDB;

// vector for single user message
vector<int> SingleUsrMGIDX;
vector<int> SingleUsrUDMGIDX;
// list buffer
vector<int> ListBuffer;

time_t now = time(0);

string MakeMessage(int ID)
{
	string Mg = "";
	Mg = to_string(MGDB[ID].m_nUID) + " " + to_string(MGDB[ID].timestamp) + " " + MGDB[ID].m_nSender + " " + MGDB[ID].m_nReceiver + " " + to_string(MGDB[ID].isRead) + " |>" + MGDB[ID].mg;
	cout<< Mg <<endl;
	return Mg;
}

// Read the users list from the local file
int LoadUserInfo(const string filename)
{
	cout<<"Loading the User Info DataBase......\n";
	ifstream fobj(filename);
	string str;
	while (getline(fobj, str))
	{
		istringstream input(str);
		string str1, str2;
		input >> str1 >> str2;
		USER user;
		user.usr = str1;
		user.pwd = str2;
		UserList.push_back(user);
	}
	cout<<"Loading done!\n";
	return 1;
}

// Save te users list to the local file
int SaveUserInfo(string filename)
{
	cout<<"Saving the User Info DataBase.......\n";
	ofstream fobj;
	fobj.open(filename, ios_base::out | ios_base::trunc);
	for (int i = 0; i < UserList.size(); i++)
	{
		string str = "";
		str = UserList[i].usr + " " + UserList[i].pwd;
		fobj << str << std::endl;
	}
	fobj.close();
	cout<<"Saving done!\n";
	return 1;
}

// Read the messages from the local file
int LoadMessageDB(const string filename)
{
	cout<<"Loading the Message DataBase......\n";
	ifstream fobj(filename);
	string str;
	while (getline(fobj, str))
	{
		if (str.empty())
		{
			continue;
		}
		istringstream input(str);
		string u_id, t, sender, receiver, iR, m;
		input >> u_id >> t >> sender >> receiver >> iR;
		MESSAGE message;
		message.m_nUID = stoi(u_id);
		message.timestamp = stoi(t);
		message.m_nSender = sender;
		message.m_nReceiver = receiver;
		message.isRead = stoi(iR);

		int pos = str.find("|>") + 2;
		m = str.substr(pos, str.length() - pos);
		message.mg = m;
		MGDB.push_back(message);
	}
	cout<<"Loading done!\n";
	return 1;
}

// Save te messages to the local file
int SaveMessageDB(string filename)
{
	cout<<"Saving the Message database......\n";
	ofstream fobj;
	fobj.open(filename, ios_base::out | ios_base::trunc);
	for (int i = 0; i < MGDB.size(); i++)
	{
		string str = MakeMessage(i);
		fobj << str<<endl;
	}
	fobj.close();
	cout<<"Saving done\n";
	return 1;
}

// the system call fail message
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void ConnectCondition(int n)
{
	if (n < 0)
	{
		error("ERROR writing/reading from socket");
	}
}

void CheckRead(int sockfd, char buffer[256])
{
	int count = 0;
	while(count<100)
	{
		bzero(buffer, 256);
		ConnectCondition(read(sockfd, buffer, 255));
		if (strlen(buffer)>0)
		{
			cout<<"Receive buffer:"<<buffer;
			break;
		}
		count++;
		cout<<".";
	}
}


int MatchUser(string usr, string pwd)
{
	int RECODE;
	//cout<< "Search:"<<usr;
	//cout<< "Password:"<<pwd<<endl;
	for (int i = 0; i < UserList.size(); i++)
	{
		//cout<<"User"<< i << ":" <<UserList[i].usr;
		//cout<<", pwd:"<< UserList[i].pwd <<endl;
		if (usr == UserList[i].usr)
		{
			if (pwd == UserList[i].pwd) // login success
			{
				RECODE = 1;
				break;
			}
			else // login fail
			{
				RECODE = 0;
			}
		}
		else // login fail
		{
			RECODE = -1;
		}
	}

	return RECODE;
}

void DispMessage(int sockfd, int ID)
{
	string buffer = MakeMessage(ID);
	ConnectCondition(write(sockfd, buffer.c_str(), 256));
}

int DispMessage(int sockfd)
{
	string MessageNum = to_string(MGDB.size());
	ConnectCondition(write(sockfd, MessageNum.c_str(), 256));
	cout<<MessageNum<<endl;
	for (int i = 0; i < MGDB.size(); i++)
	{
		cout<<"Sent " << i << ":";
		DispMessage(sockfd, i);
	}
	return 1;
}

int DispMessage(int sockfd, string user, int sr)
{
	if (sr == 1)
	{
		for (int i = 0; i < MGDB.size(); i++)
		{
			if (user == MGDB[i].m_nSender)
			{
				ListBuffer.push_back(i);
			}
		}
	}
	else
	{
		for (int i = 0; i < MGDB.size(); i++)
		{
			if (user == MGDB[i].m_nReceiver)
			{
				ListBuffer.push_back(i);
			}
		}
	}

	string MessageNum = to_string(ListBuffer.size());
	ConnectCondition(write(sockfd, MessageNum.c_str(), 256));
	cout<<"The nume of messgae:"<<MessageNum<<endl;
	for (int i = 0; i < ListBuffer.size(); i++)
	{
		cout<<"Sent " << i << ": ";
		DispMessage(sockfd, ListBuffer[i]);
	}
	ListBuffer.clear();
	return 1;
}

int DisplayMessage(int sockfd)
{
	char buffer[256];
	string usr;
	// receive the message from the user
	CheckRead(sockfd, buffer);
	int opt = atoi(buffer);
	string MessageNum;
	switch (opt)
	{
	case 1:
		DispMessage(sockfd);
		break;
	case 2:
		MessageNum = to_string(MGDB.size());
		ConnectCondition(write(sockfd, MessageNum.c_str(), 256));
		CheckRead(sockfd, buffer);
		DispMessage(sockfd, atoi(buffer));
		break;
	case 3:
		CheckRead(sockfd, buffer);
		buffer[strlen(buffer)-1] = 0;
		usr = buffer;
		DispMessage(sockfd, usr , 1);
		break;
	case 4:
		CheckRead(sockfd, buffer);
		buffer[strlen(buffer)-1] = 0;
		usr = buffer;
		DispMessage(sockfd, usr, 2);
		break;
	}

	return 1;
}

int DeleteMessage(int sockfd)
{
	char buffer[256];

	// receive the message from the user
	CheckRead(sockfd, buffer);

	int opt = atoi(buffer);
	if (opt == 1)
	{
		MGDB.clear();
	}
	else
	{
		string MessageNum = to_string(MGDB.size());
		ConnectCondition(write(sockfd, MessageNum.c_str(), 256));
		CheckRead(sockfd, buffer);
		MGDB.erase(MGDB.begin() + atoi(buffer));
	}

	return 1;
}

int CreateAccount(int sockfd)
{
	char buffer[256];
	// receive the message from the user
	CheckRead(sockfd, buffer);
	buffer[strlen(buffer)-1] = 0;
	string newUserName = buffer;

	CheckRead(sockfd, buffer);
	buffer[strlen(buffer)-1] = 0;
	string newPassword = buffer;

	// Account Create Success
	if (MatchUser(newUserName, newPassword) == -1)
	{
		cout<<"Create Success\n";
		USER user;
		user.usr = newUserName;
		user.pwd = newPassword;
		UserList.push_back(user);
		ConnectCondition(write(sockfd, "1", 1));
	}
	else // Account Create Wrong
	{
		cout<<"Create Fail\n";
		ConnectCondition(write(sockfd, "0", 1));
	}

	return 1;
}

int RemoveAccount(int sockfd)
{
	char buffer[256];

	// receive the message from the user
	CheckRead(sockfd, buffer);
	buffer[strlen(buffer)-1] = 0;
	string newUserName = buffer;

	// Account Remove Success
	if (MatchUser(newUserName," ") == 1)
	{
		for (int i = 0; i < UserList.size(); i++)
		{
			if (newUserName == UserList[i].usr)
			{	
				UserList.erase(UserList.begin() + i);
				break;
			}
		}
		cout<<"Account delete success"<<endl;
		ConnectCondition(write(sockfd, "1", 1));
	}
	else // Account Remove Wrong
	{
		cout<<"Account delete fail"<<endl;
		ConnectCondition(write(sockfd, "0", 1));
	}

	return 1;
}

int AdminManage(int sockfd)
{
	cout<< "Hello, admin"<<endl;
	char buffer[256];

	int count = 1;
	int OperateCode = -1;
	while (count < 50)
	{
		CheckRead(sockfd, buffer);
		OperateCode = atoi(buffer);
		switch (OperateCode)
		{
		case 0:
			count = 100;
			break;
		case 1:
			CreateAccount(sockfd);
			count++;
			break;
		case 2:
			RemoveAccount(sockfd);
			count++;
			break;
		case 3:
			DisplayMessage(sockfd);
			count++;
			break;
		case 4:
			DeleteMessage(sockfd);
			count++;
			break;
		}
	}

	return 1;
}

// collect the message with current user
int CollectMessage(string usrname)
{
	for (int i = 0; i < MGDB.size(); i++)
	{
		if (usrname == MGDB[i].m_nReceiver)
		{
			SingleUsrMGIDX.push_back(i);
			if (MGDB[i].isRead == 0)
			{
				SingleUsrUDMGIDX.push_back(i);
			}
		}
	}
	return 1;
}

int SendMessage(int sockfd)
{
	char buffer[256];

	// receive the message from the user
	CheckRead(sockfd, buffer);

	int opt = atoi(buffer);
	switch (opt)
	{
	case 1: // send message to all users
		CheckRead(sockfd, buffer);
		buffer[strlen(buffer)-1] = 0;
		now = time(0);
		for (int i = 0; i < UserList.size(); i++)
		{
			MESSAGE message;
			message.m_nUID = MGDB.back().m_nUID + 1;
			message.timestamp = now;
			message.m_nSender = UserName;
			message.m_nReceiver = UserList[i].usr;
			message.isRead = 0;	
			message.mg = buffer;
			MGDB.push_back(message);
		}
		break;
	case 2: // send message to particular usr
		CheckRead(sockfd, buffer);
		now = time(0);
		MESSAGE message;
		message.m_nUID = MGDB.back().m_nUID + 1;
		message.timestamp = now;
		message.m_nSender = UserName;
		buffer[strlen(buffer)-1] = 0;
		message.m_nReceiver = buffer;
		message.isRead = 0;

		CheckRead(sockfd, buffer);
		message.mg = string(buffer);
		MGDB.push_back(message);

		break;
	}

	return 1;
}

int ReadMessage(int sockfd)
{
	char buffer[256];
	CheckRead(sockfd, buffer);
	int opt = atoi(buffer);
	string MessageNum;
	switch (opt)
	{
	case 1: // read message
		MessageNum = to_string(SingleUsrUDMGIDX.size());
		cout<< MessageNum << " messages need to be send."<<endl;
		ConnectCondition(write(sockfd, MessageNum.c_str(), 256));
		for (int i = 0; i < SingleUsrUDMGIDX.size(); i++)
		{
			DispMessage(sockfd, SingleUsrUDMGIDX[i]);
			MGDB[SingleUsrUDMGIDX[i]].isRead = 1;
		}
		break;
	case 2: // Read all message to particular usr
		MessageNum = to_string(SingleUsrMGIDX.size());
		cout<< MessageNum << " message need to be send."<<endl;
		ConnectCondition(write(sockfd, MessageNum.c_str(), 256));
		for (int i = 0; i < SingleUsrMGIDX.size(); i++)
		{
			DispMessage(sockfd, SingleUsrMGIDX[i]);
		}
		break;
	}

	return 1;
}

int UserManage(int sockfd)
{
	cout<< "Hello,"<< UserName <<endl;
	int count = 1;
	char buffer[256];
	int OperateCode;
	while (count < 100)
	{
		CollectMessage(UserName);
		CheckRead(sockfd, buffer);
		OperateCode = atoi(buffer);
		switch (OperateCode)
		{
		case 0:
			count = 101;
			break;
		case 1:
			SendMessage(sockfd);
			count++;
			break;
		case 2:
			ReadMessage(sockfd);
			count++;
			break;
		}
		SingleUsrMGIDX.clear();
		SingleUsrUDMGIDX.clear();
	}

	return 1;
}

int LoginMailBox(int sockfd)
{
	cout<< "Login the mailbox"<<endl;
	int count;
	int RECODE;
	char buffer[256];
	// receive the message from the user
	CheckRead(sockfd, buffer);
	buffer[strlen(buffer)-1] = 0;
	UserName = buffer;

	CheckRead(sockfd, buffer);
	buffer[strlen(buffer)-1] = 0;
	Password = buffer;
	//cout<<Password;
	// match username
	if (MatchUser(UserName, Password) == 1)
	{
		cout <<"Right\n";
		ConnectCondition(write(sockfd, "1", 1));
		RECODE = 1;
	}
	else // send wrong to client
	{
		cout <<"Wrong\n";
		ConnectCondition(write(sockfd, "0", 1));
		RECODE = 0;
	}

	return RECODE;
}

int main(int argc, char *argv[])
{
	// read the users file
	string UserInfoFileName = "UserInfo.txt";
	LoadUserInfo(UserInfoFileName);

	// read the message file
	string MessageDBFileName = "MessageDB.txt";
	LoadMessageDB(MessageDBFileName);

	// the file descriptor from socket and accept system call
	int sockfd, newsockfd;
	// port ID
	int PortID;

	socklen_t clilen;
	// address of the server and client
	struct sockaddr_in serv_addr, client_addr;

	//
	if (argc < 2)
	{
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}

	// create a socket
	// AF_INET: internet domain
	// SOCK_STREAM: data format(maybe changed to SOCK_DGRAM)
	// protocol: 0(TCP)
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// return error message
	if (sockfd < 0)
	{
		error("ERROR opening socket");
	}

	// initial the server address to all 0s
	bzero((char *)&serv_addr, sizeof(serv_addr));

	// obtain port ID
	PortID = atoi(argv[1]);

	// set server address
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY; // IP address
	serv_addr.sin_port = htons(PortID);		// port ID

	// bind the socket which has the address of server
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		error("ERROR on binding");
	}

	// listen the socket for connection. maximum try is 5
	listen(sockfd, 5);

	// create the new socket when the client is connected.
	clilen = sizeof(client_addr);
	newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &clilen);

	// return error message
	if (newsockfd < 0)
	{
		error("ERROR on accept");
	}

	int count = 0;
	char buffer[256];
	while (count < 100)
	{
		CheckRead(newsockfd, buffer);
		if (atoi(buffer) == 1)
		{
			if (LoginMailBox(newsockfd) == 1)
			{
				if (UserName == "admin")
				{
					AdminManage(newsockfd);
				}
				else
				{
					UserManage(newsockfd);
				}
				count++;
			}
			else
			{
				count++;
			}
		}
		else
		{
			count = 101;
		}
	}

	// close the socket
	close(newsockfd);
	close(sockfd);

	// save the data into the file
	SaveUserInfo(UserInfoFileName);
	SaveMessageDB(MessageDBFileName);
	return 0;
}
