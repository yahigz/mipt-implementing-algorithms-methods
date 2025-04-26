#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

using ll = long long;

struct Job {
    ll r;
    ll d;
    ll inc;
};

class IncomeMaximizer{ 
    private:
        std::vector<Job> jobs_;
        const ll INF = 2'000'000'000'000'000'000;
        int n_;
    public:
        IncomeMaximizer(std::vector<Job> jobs): n_(jobs.size()), jobs_(jobs) {}

        std::pair<ll, std::vector<int>> Calc() {
            if (jobs_.empty()) {
                return {0, {}};
            }
            std::vector<ll> coor;
            for (auto elem : jobs_) {
                coor.push_back(elem.r);
                coor.push_back(elem.d);
                coor.push_back(elem.d + 1);
            }
            sort(coor.begin(), coor.end());
            coor.resize(unique(coor.begin(), coor.end()) - coor.begin());

            std::map<ll, int> coor_to_ind;
            for (int i = 0; i < coor.size(); ++i) {
                coor_to_ind[coor[i]] = i;
            }

            std::vector<std::vector<int>> list_of_jobs(coor.size());

            for (int i = 0; i < jobs_.size(); ++i) {
                list_of_jobs[coor_to_ind[jobs_[i].r]].push_back(i);
            }

            std::vector<std::pair<ll, int>> dp(coor.size(), {0, -1});
            dp[0] = {0, -1};
            for (int i = 0; i < coor.size(); ++i) {
                if (i > 0 && dp[i - 1].first > dp[i].first) {
                    dp[i] = dp[i - 1];
                }
                for (auto ind : list_of_jobs[i]) {
                    if (dp[coor_to_ind[jobs_[ind].d]].first < dp[i].first + jobs_[ind].inc) {
                        dp[coor_to_ind[jobs_[ind].d]] = {dp[i].first + jobs_[ind].inc, ind}; 
                    }
                }
            }

            ll ans = -INF;
            int ind_ans = -1;
            for (int i = 0; i < coor.size(); ++i) {
                if (dp[i].first > ans) {
                    ans = dp[i].first;
                    ind_ans = i;
                }
            }
            std::vector<int> indexes;
            while (true) {
                if (dp[ind_ans].second == -1) {
                    break;
                } 
                indexes.push_back(dp[ind_ans].second + 1);
                ind_ans = coor_to_ind[jobs_[dp[ind_ans].second].r];
            }
            
            reverse(indexes.begin(), indexes.end());

            return {ans, indexes};
        }
};  

int main() {
    int n;
    std::cin >> n;
    std::vector<Job> a(n);
    for (auto& elem : a) {
        std::cin >> elem.r >> elem.d >> elem.inc;
    }
    auto [ans, indexes] = IncomeMaximizer(a).Calc();
    std::cout << ans << ' ' << indexes.size() << '\n';
    for (auto elem : indexes) {
        std::cout << elem << ' ';
    }
    std::cout << '\n';
}