// ProjectJeriJson.cpp: 定义应用程序的入口点。
//

#include "ProjectJeriJson.h"
#include "JeriJson/JeriJson.h"

using namespace std;

int main() {
	string str = "this is a string";
	JeriJson::JObject* ptr = JeriJson::JObject::Parse(str);
	printf("ptr = %p\n", ptr);
	cout << "Hello CMake." << endl;
	return 0;
}
