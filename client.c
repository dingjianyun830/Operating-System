#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>

using namespace std;
// Current Client
string UserName = "";
string Password = "";

// Print the message to guide the user
void HelpMessage(int opt)
{
	if (opt == 1)
	{
		cout << "|*--------------------------------------------------*|\n";
		cout << "Hello, welcome to the YUKI MAILBOX\n";
		cout << "Use the option number to login/Leave the mailbox\n";
		cout << "1--Login\n";
		cout << "0--Exit\n";
		cout << "|*--------------------------------------------------*|\n";
	}
	else if (opt == 2)
	{
		cout << "|*--------------------------------------------------*|\n";
		cout << "Hello, Administrator\n";
		cout << "Input your option:\n";
		cout << "1--Create Account\n";
		cout << "2--Delete Account\n";
		cout << "3--Read Message\n";
		cout << "4--Delete Message\n";
		cout << "0--Exit\n";
		cout << "|*--------------------------------------------------*|\n";
	}
	else if (opt == 3)
	{
		cout << "|*------------------------------*|\n";
		cout << "Hello, Administrator\n";
		cout << "Read the Message, please enter the code:\n";
		cout << "1--Read All Message\n";
		cout << "2--Read Single Message with ID\n";
		cout << "3--Read the Message with Sender\n";
		cout << "4--Read the Message with Receiver\n";
		cout << "0--Exit\n";
		cout << "|*--------------------------------------------------*|\n";
	}
	else if (opt == 4)
	{
		cout << "|*--------------------------------------------------*|\n";
		cout << "Hello, Administrator\n";
		cout << "Delete the Messaage, please enter the code:\n";
		cout << "1--Delete All Message\n";
		cout << "2--Delete Single Message with ID\n";
		cout << "0--Exit\n";
		cout << "|*--------------------------------------------------*|\n";
	}
	else if (opt == 5)
	{
		cout << "|*--------------------------------------------------*|\n";
		cout << "Hello, Dear " << UserName.c_str() << endl;
		cout << "Read/Send the Message, please enter the code:\n";
		cout << "1--Send Message\n";
		cout << "2--Read Message\n";
		cout << "0--Exit\n";
		cout << "|*--------------------------------------------------*|\n";
	}
	else if (opt == 6)
	{
		cout << "|*--------------------------------------------------*|\n";
		cout << "Hello, Dear " << UserName.c_str() << endl;
		cout << "Send the Message, please enter the code:\n";
		cout << "1--Send Message to ALL user\n";
		cout << "2--Send Message to single user\n";
		cout << "0--Exit\n";
		cout << "|*--------------------------------------------------*|\n";
	}
	else if (opt == 7)
	{
		cout << "|*--------------------------------------------------*|\n";
		cout << "Hello, Dear " << UserName.c_str() << endl;
		cout << "Read the message, please enter the code:\n";
		cout << "1--Read unread Message\n";
		cout << "2--Read All message\n";
		cout << "0--Exit\n";
		cout << "|*--------------------------------------------------*|\n";
	}
}

// the system call fail message
void error(const char *msg)
{
	perror(msg);
	exit(0);
}

void ConnectCondition(int n)
{
	if (n < 0)
	{
		error("ERROR writing/reading to socket");
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


int LoginMailBox(int sockfd)
{
	char buffer[256];
	cout << "Please enter the username:" << endl;
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	ConnectCondition(write(sockfd, buffer, 256));
	buffer[strlen(buffer) - 1] = 0;
	UserName = buffer;

	cout << "Please enter the password:" << endl;
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	ConnectCondition(write(sockfd, buffer, 256));
	buffer[strlen(buffer) - 1] = 0;
	Password = buffer;

	bzero(buffer, 256);
	ConnectCondition(read(sockfd, buffer, 255));
	return atoi(buffer);
}

int CreateAccount(int sockfd)
{

	char buffer[256];
	cout << "Please enter the username:" << endl;
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	ConnectCondition(write(sockfd, buffer, 256));

	cout << "Please enter the password:" << endl;
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	ConnectCondition(write(sockfd, buffer, 256));

	bzero(buffer, 256);
	ConnectCondition(read(sockfd, buffer, 255));

	return atoi(buffer);
}

int RemoveAccount(int sockfd)
{
	char buffer[256];
	cout << "Please enter the username:";
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	ConnectCondition(write(sockfd, buffer, 256));

	bzero(buffer, 256);
	ConnectCondition(read(sockfd, buffer, 255));
	return atoi(buffer);
}

void DispMessageSingle(int sockfd)
{
	char buffer[256];
	bzero(buffer, 256);
	ConnectCondition(read(sockfd, buffer, 256));
	cout << buffer << endl;
}

void DispMessage(int sockfd)
{
	char buffer[256];
	bzero(buffer, 256);
	ConnectCondition(read(sockfd, buffer, 256));
	cout << "The num of message:" << buffer << endl;
	int MessageNum = atoi(buffer);
	for (int i = 0; i < MessageNum; i++)
	{
		DispMessageSingle(sockfd);
	}
}

void ReadMessage(int sockfd)
{
	HelpMessage(3);
	char buffer[256];
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	ConnectCondition(write(sockfd, buffer, 256));
	int opt = atoi(buffer);
	switch (opt)
	{
	case 1:
		DispMessage(sockfd);
		break;
	case 2:
		bzero(buffer, 256);
		ConnectCondition(read(sockfd, buffer, 255));
		cout << "The ID scope is [0~" << buffer << "]" << endl;
		cout << "Please enter the ID:" << endl;
		bzero(buffer, 256);
		fgets(buffer, 255, stdin);
		ConnectCondition(write(sockfd, buffer, 256));
		DispMessageSingle(sockfd);
		break;
	case 3:
		cout << "Please enter the Sender:" << endl;
		bzero(buffer, 256);
		fgets(buffer, 255, stdin);
		ConnectCondition(write(sockfd, buffer, 256));
		DispMessage(sockfd);
		break;
	case 4:
		cout << "Please enter the Receiver:" << endl;
		bzero(buffer, 256);
		fgets(buffer, 255, stdin);
		ConnectCondition(write(sockfd, buffer, 256));
		DispMessage(sockfd);
		break;
	}
}

void DeleteMessage(int sockfd)
{
	HelpMessage(4);
	char buffer[256];
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	ConnectCondition(write(sockfd, buffer, 256));
	int opt = atoi(buffer);
	switch (opt)
	{
	case 1:
		cout << "All messgaes have been deleted." << endl;
	case 2:
		bzero(buffer, 256);
		ConnectCondition(read(sockfd, buffer, 255));
		cout << "The ID scope is [0 :" << buffer << "]" << endl;
		cout << "Please enter the ID:" << endl;
		bzero(buffer, 256);
		fgets(buffer, 255, stdin);
		ConnectCondition(write(sockfd, buffer, 256));
	}
}

int AdminManage(int sockfd)
{
	char buffer[256];
	int count = 0;
	while (count < 100)
	{
		HelpMessage(2);
		bzero(buffer, 256);
		fgets(buffer, 255, stdin);
		int opt = atoi(buffer);
		ConnectCondition(write(sockfd, buffer, 256));
		switch (opt)
		{
		case 0:
			count = 101;
			break;
		case 1:
			if (CreateAccount(sockfd))
			{
				cout << "Create Account Success\n";
			}
			else
			{
				cout << "Create Account Fail\n";
			}
			count++;
			break;
		case 2:
			if (RemoveAccount(sockfd))
			{
				cout << "Delete Account Success\n";
			}
			else
			{
				cout << "Delete Account Fail\n";
			}
			count++;
			break;
		case 3:
			ReadMessage(sockfd);
			count++;
			break;
		case 4:
			count++;
			DeleteMessage(sockfd);
			break;
		}
	}

	return 1;
}

int UserManage(int sockfd)
{
	char buffer[256];
	int count = 0;
	while (count < 100)
	{
		HelpMessage(5);
		bzero(buffer, 256);
		fgets(buffer, 255, stdin);
		ConnectCondition(write(sockfd, buffer, 256));
		int opt = atoi(buffer);
		switch (opt)
		{
		case 0:
			count = 101;
			break;
		case 1:
			HelpMessage(6);
			bzero(buffer, 256);
			fgets(buffer, 255, stdin);
			ConnectCondition(write(sockfd, buffer, 256));
			if (atoi(buffer) == 1)
			{
				cout << "Please enter the message:" << endl;
				bzero(buffer, 256);
				fgets(buffer, 255, stdin);
				ConnectCondition(write(sockfd, buffer, 256));
			}
			else
			{
				cout << "Please enter the username:" << endl;
				bzero(buffer, 256);
				fgets(buffer, 255, stdin);
				ConnectCondition(write(sockfd, buffer, 256));
				cout << "Please enter the message:" << endl;
				bzero(buffer, 256);
				fgets(buffer, 255, stdin);
				ConnectCondition(write(sockfd, buffer, 256));
			}
			count++;
			break;
		case 2:
			HelpMessage(7);
			bzero(buffer, 256);
			fgets(buffer, 255, stdin);
			ConnectCondition(write(sockfd, buffer, 256));
			DispMessage(sockfd);
			count++;
			break;
		}
	}

	return 1;
}

int main(int argc, char *argv[])
{
	// file discriptor from socket
	int sockfd;

	// the port ID
	int PortID;

	// the return value of read() and write()
	int n;

	// the address of server
	struct sockaddr_in serv_addr;
	// define a host computer
	struct hostent *server;

	char buffer[256];

	// the client should input the host name and the portID
	// use commend hostname to get the host name
	if (argc < 3)
	{
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0);
	}

	// obtain the port ID
	PortID = atoi(argv[2]);

	// create a socket
	// AF_INET: internet domain
	// SOCK_STREAM: data format(maybe changed to SOCK_DGRAM)
	// protocol: 0(TCP)
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		error("ERROR opening socket");
	}

	// obtain the host by input host name
	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}

	// initial the server address
	bzero((char *)&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

	// Connect the host with the port ID
	serv_addr.sin_port = htons(PortID);
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		error("ERROR connecting");
	}

	int count = 0;
	while (count < 100)
	{
		HelpMessage(1);
		bzero(buffer, 256);
		fgets(buffer, 255, stdin);
		ConnectCondition(write(sockfd, buffer, 256));
		int opt = atoi(buffer);
		switch (opt)
		{
		case 1:
			if (LoginMailBox(sockfd) == 1)
			{
				cout << "Login Success.\n";
				if (UserName == "admin")
				{
					AdminManage(sockfd);
				}
				else
				{
					UserManage(sockfd);
				}
			}
			else
			{
				cout << "Login Fail. Please check your username and password.\n";
			}
			count++;
			break;
		case 0:
			cout << "See you again. Bye.\n";
			count = 101;
			break;
		default:
			printf("The option number is wrong. Please re-enter");
			count++;
			break;
		}
	}

	close(sockfd);
	return 0;
}
