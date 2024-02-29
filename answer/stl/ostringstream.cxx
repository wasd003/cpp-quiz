#include <quiz/base.h>
#include <string>  
#include <iostream>  
#include <sstream>
using namespace std;  

int main() {  
    std::ostringstream ostr1;
  
    ostr1 << "ostr1 " << 2012 << endl; // 1. append的方法
    cout << ostr1.str();   
  
    long curPos = ostr1.tellp(); // 2. 获得末尾指针
    UNUSED(curPos);
    ostr1.seekp(2); // 3. 修改末尾指针
    ostr1 << "xxxxx";
    cout << ostr1.str() << endl; // 4. 获得字符串
      
    return 0;
}
