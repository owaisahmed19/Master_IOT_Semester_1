#include <QCoreApplication>
#include <iostream>
#include <string>
using namespace std;





void staticCounter() {
    static int c = 0; // created once, keeps value
    c++;
    cout << c << endl;
}

class Pizza {
public:
    static int count;  // shared by all Pizza objects
    Pizza() { count--; }


};

int Pizza::count = 0;  // define it outside


int main(int argc, char *argv[])
{

    int x=10;
    int *px=&x;
//extern int globalSize;
   //  std::cout<<globalSize<<std::endl;
    string pizza[5]={"pizza1","pizza2","pizza3","pizza4","pizza5"};

    string *pPizza=pizza;
   // globalSize=100;
   // std::cout<<globalSize<<std::endl;


    //const int a=1000;


   // for(int i=0;i<size(*pPizza);i++){
    //    std::cout << *(pPizza+i)<<std::endl;
    //}


  //  cout << "Normal:" << endl;
   // staticCounter(); // 1
  //  staticCounter(); // 1 again
  //  staticCounter(); // 1 again


   // Pizza p1, p2;
  //  cout << Pizza::count << endl; // prints 3


    //int data=3;
    //int name=(data==5)?4:5;
    //cout<<name<<endl;


    /*
     *     int b=1;
    switch(b){
    case 20:cout<<"10"<<endl;
        break;
    case 1:cout<<"1"<<endl;
        break;
    }
        int a=2;
        int& refa=a;
        std::cout << "refa=" << refa << std::endl;
        refa = 3;
        std::cout << "a=" << a << std::endl;
        int taba[5]={2,5,3,4,1};
        refa=taba[4];
        std::cout << "refa=" << refa << std::endl;
     */


    //reference to variable

    int var=100;
    int& varRef=var;

    const int varc=100;
    const int& varcRef=varc;

    int *pvar=&var;
    int*& ref=pvar;


    cout<<*pvar<<endl;





    return 0;
}
