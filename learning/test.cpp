#include <iostream>  
#include <string>  
#include <vector>  
using namespace std;  
void vectorTest()  
{  
    vector<int> container;  
    for (int i = 0; i < 10; i++)  
    {  
        container.push_back(i);  
    }  

    vector<int>::iterator iter;  
    for (iter = container.begin(); iter != container.end(); iter++)//循环2  
    {  
        container.erase(iter);  
    }  

    for (iter = container.begin(); iter != container.end(); iter++)  
    {  
        cout<<*iter<<" ";  
    }  
    cout<<endl;  
}  
int main(int argc, const char *argv[])  
{  
    vectorTest();  
    return 0;  
}  
