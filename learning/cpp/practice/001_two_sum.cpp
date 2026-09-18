// TWO SUM (LeetCode #1, Easy) - the canonical first problem
// Given nums[] and target, return indices of the two numbers that add to target.
//
// BRUTE FORCE: try every pair.  O(n^2)
// HASH MAP:    one pass, remember what you've seen.  O(n)
#include <bits/stdc++.h>
using namespace std;

vector<int> twoSumBrute(vector<int>& nums, int target) {
    for (int i = 0; i < (int)nums.size(); i++)
        for (int j = i + 1; j < (int)nums.size(); j++)
            if (nums[i] + nums[j] == target) return {i, j};
    return {};
}

vector<int> twoSumFast(vector<int>& nums, int target) {
    unordered_map<int,int> seen;            // value -> index
    for (int i = 0; i < (int)nums.size(); i++) {
        int need = target - nums[i];        // the partner we're looking for
        if (seen.count(need)) return {seen[need], i};
        seen[nums[i]] = i;                  // remember this one
    }
    return {};
}

int main() {
    vector<int> nums = {2, 7, 11, 15};
    int target = 9;
    vector<int> a = twoSumBrute(nums, target);
    vector<int> b = twoSumFast(nums, target);
    cout << "brute: [" << a[0] << "," << a[1] << "]\n";
    cout << "hash : [" << b[0] << "," << b[1] << "]\n";
    return 0;
}
