#ifndef _CONNECT_DATA_BASE_H
#define _CONNECT_DATA_BASE_H
#include<mysql.h>
#include<string>
using namespace std;

class Connection{
public:
	Connection(){
		mysql_init(&conn);
	}
	void connect();
	void update();
	bool updateUser();
	void updatefriendmp();
	void updategroupmp();
	~Connection(){
		mysql_close(&conn);
	}
private:
	MYSQL conn;
	
};
#endif
