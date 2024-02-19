//
// Created by bokket on 2024/1/31.
//

#include "../db/status.h"

using namespace std;
using namespace bokket;
int main()
{
    DB code=DB::OK;
    if (code !=DB::NotFound) {
        cout<<Status(code)<< endl;
    }
}