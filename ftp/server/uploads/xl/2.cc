#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <cmath>
#include <ctype.h>
#include <stdio.h>
#include <forward_list>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
// const std::string COLOUR1 = "\033[1;36m";
// const std::string COLOUR2 = "\033[0m";
using namespace std;
// class Solution
// {
// public:
//     int longestMountain(vector<int> &arr)
//     {
//         int n = arr.size();
//         if (!n)
//         {
//             return 0;
//         }
//         vector<int> left(n);
//         for (int i = 1; i < n; ++i)
//         {
//             left[i] = (arr[i - 1] < arr[i] ? left[i - 1] + 1 : 0);
//         }
//         vector<int> right(n);
//         for (int i = n - 2; i >= 0; --i)
//         {
//             right[i] = (arr[i + 1] < arr[i] ? right[i + 1] + 1 : 0);
//         }

//         int ans = 0;
//         for (int i = 0; i < n; ++i)
//         {
//             if (left[i] > 0 && right[i] > 0)
//             {
//                 ans = max(ans, left[i] + right[i] + 1);
//             }
//         }
//         return ans;
//     }
// };
// // class Solution
// // {
// // public:
// //     int longestMountain(vector<int> &arr)
// //     {
// //         int maxlen = 0, count = 0, left = 1;
// //         bool flag = true;
// //         for (int i = 1; i < arr.size(); i++)
// //         {
// //             if (arr[i] > arr[i - 1])
// //             {
// //                 count++;
// //                 if (flag == false)
// //                 {
// //                     maxlen = max(maxlen, count);
// //                     count = 0;
// //                 }
// //             }
// //             else if (arr[i] < arr[i - 1])
// //             {
// //                 if (i == left)
// //                 {
// //                     left = ++i;
// //                     continue;
// //                 }
// //                 count++;
// //                 flag = false;
// //             }
// //         }
// //         return maxlen;
// //     }
// // };
// std::string segstrspace(std::string &order, int count = 0)
// {
//     while (count < order.size())
//     {
//         if ((order[count] == ' ' && count == 0) || (order[count] == ' ' && count == order.size() - 1) || (count + 1 < order.size() && order[count] == ' ' && order[count + 1] == ' '))
//         {
//             order.erase(count, 1);
//         }
//         else
//         {
//             count++;
//         }
//     }
//     return order;
// }
// class as{
//     int val;
//     void valchgange(int a);
// };
// void as::valchgange(int a){
//     val = a;
// }
// int main()
// {
//     string a = "hhh.cc";
//     // stor(a, 1);
//     return 0;
// }
class Solution
{
public:
    int lengthOfLongestSubstring(string s)
    {
        int n = s.length(), count = 0, left = 0, maxnum = 0;
        unordered_map<char, int> hasht;
        for (int right = 0; right < n; right++)
        {
            char c = s[right];
            hasht[c]++;
            while (hasht[c] > 1)
            {
                count--;
                hasht[s[left]]--;
                left++;
            }
            count++;
            if (count > maxnum)
            {
                maxnum = count;
            }
        }
        return count;
    }
};

int main(void)
{

    return 0;
}
