#include <iostream>
#include <fstream>
#include "PublicTransport.h"
using namespace std;
int main()
{
	ifstream file2("file.txt");
	PublicTransport q(file2);
	file2.close();
	cout << endl << endl;
	q.menu();
	
	return 0;
}