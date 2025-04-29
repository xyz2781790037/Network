#include <bits/stdc++.h>
using namespace std;
const int maxn = 1e3 + 10;
int n, a[maxn];
int ans = INT_MAX;
int minnum(int a, long long b)
{
    return a > b ? b : a;
}
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(0), cout.tie(0);
    cin >> n;
    for (int i = 1; i <= n; i++)
        cin >> a[i];
    for (int i = 1; i <= n; i++)
        for (int j = i + 1; j <= n; j++)
        {
            int l = i - 1, r = j + 1;
            long long suml = a[i], sumr = a[j];
            ans = minnum(ans, abs(suml - sumr));
            while (l >= 1 && r <= n)
            {
                if (suml > sumr)
                    sumr += a[r++];
                else
                    suml += a[l--];
                ans = minnum(ans, abs(suml - sumr));
            }
            while (l >= 1)
            {
                suml += a[l--];
                ans = minnum(ans, abs(suml - sumr));
            }
            while (r <= n)
            {
                sumr += a[r++];
                ans = minnum(ans, abs(suml - sumr));
            }
        }
    cout << ans << endl;
    return 0;
}