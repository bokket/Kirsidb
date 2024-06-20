//
// Created by bokket on 2024/1/31.
//

#include "../db/status.h"

using namespace std;
using namespace bokket;
int main()
{
    auto c= Status::IOError("?");

    printf("%d",static_cast<int>(c.code()));

    //c= Status::Ok();
    cout<<c.ToString()<<endl;

    auto b= Status::OK();
    cout<<c.ToString()<< endl;

    if(b)
    {
        cout<<"ok"<<endl;
    } else
        cout<<"no"<<endl;

    b=Status::IOError("hhh");

    if(b)
    {
        cout<<"ok"<<endl;
    } else
        cout<<"no"<<endl;

    cout<< b.ToString()<<static_cast<int>(b.code())<< endl;

    auto ret=(c.code()<b.code()) ? 1:0;
    cout<< ret<< endl;
}