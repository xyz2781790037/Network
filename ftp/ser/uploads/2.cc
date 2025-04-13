#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <cstring>
#include <cmath>
#include <ctype.h>
#include <stdio.h>
#include <forward_list>
#include <unordered_map>
#include <vector>
const std::string COLOUR1 = "\033[1;36m";
const std::string COLOUR2 = "\033[0m";
using namespace std;
class Solution
{
public:
    int longestMountain(vector<int> &arr)
    {
        int n = arr.size();
        if (!n)
        {
            return 0;
        }
        vector<int> left(n);
        for (int i = 1; i < n; ++i)
        {
            left[i] = (arr[i - 1] < arr[i] ? left[i - 1] + 1 : 0);
        }
        vector<int> right(n);
        for (int i = n - 2; i >= 0; --i)
        {
            right[i] = (arr[i + 1] < arr[i] ? right[i + 1] + 1 : 0);
        }

        int ans = 0;
        for (int i = 0; i < n; ++i)
        {
            if (left[i] > 0 && right[i] > 0)
            {
                ans = max(ans, left[i] + right[i] + 1);
            }
        }
        return ans;
    }
};
// class Solution
// {
// public:
//     int longestMountain(vector<int> &arr)
//     {
//         int maxlen = 0, count = 0, left = 1;
//         bool flag = true;
//         for (int i = 1; i < arr.size(); i++)
//         {
//             if (arr[i] > arr[i - 1])
//             {
//                 count++;
//                 if (flag == false)
//                 {
//                     maxlen = max(maxlen, count);
//                     count = 0;
//                 }
//             }
//             else if (arr[i] < arr[i - 1])
//             {
//                 if (i == left)
//                 {
//                     left = ++i;
//                     continue;
//                 }
//                 count++;
//                 flag = false;
//             }
//         }
//         return maxlen;
//     }
// };
std::string segstrspace(std::string &order, int count = 0)
{
    while (count < order.size())
    {
        if ((order[count] == ' ' && count == 0) || (order[count] == ' ' && count == order.size() - 1) || (count + 1 < order.size() && order[count] == ' ' && order[count + 1] == ' '))
        {
            order.erase(count, 1);
        }
        else
        {
            count++;
        }
    }
    return order;
}
int a(int &a, int &nb)
{
    nb = 3;
    return a + 1;
}

const int months[] = {
    0, 31, 28, 31, 30, 31,
    30, 31, 31, 30, 31, 30,
    31};

bool check(string str)
{
    for (int i = 0; i + 2 < str.size(); i++)
        if (str[i + 1] == str[i] + 1 && str[i + 2] == str[i] + 2)
            return true;

    return false;
}