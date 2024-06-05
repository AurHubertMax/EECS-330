#ifndef __MYNUMBER_H__
#define __MYNUMBER_H__
#include <algorithm>
#include <iostream>
using namespace std;
template <typename DataType>
class MyNumber
{
    public:
        explicit MyNumber(DataType rhs = 0) 
        {
            // code begins
            num = new DataType{rhs};
            // code ends
        }

        MyNumber(const MyNumber<DataType> & rhs) 
        {
            // code begins
            num = new DataType{*rhs.num};
            return;
            // code ends
        }

        MyNumber(MyNumber<DataType> && rhs) 
        {
            // code begins
            num = rhs.num;
            rhs.num = nullptr;
            return;
            // code ends
        }

        MyNumber & operator= (const MyNumber<DataType> &rhs) // copy assignment with 
        a MyNumber instance
        {
            // code begins
            *num = *(rhs.num);
            return *this;
            // code ends
        }
        MyNumber & operator= (MyNumber<DataType> && rhs) 
        {
            // code begins
            num = rhs.num;
            rhs.num = nullptr;
            return *this;
            // code ends
        }
        ~MyNumber(void) 
        {
            // code begins
            delete num;
            // code ends
        }

        DataType read(void) const
        { 
            // code begins
            return *num;
            // code ends
        }
        void write(DataType rhs)
        {

            *num = rhs;

        }
    private:

    DataType *num;
};
#endif 