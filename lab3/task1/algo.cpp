#include <algorithm>
#include <iostream>
#include <vector>

using ll = long long;

struct Job {
    ll inc;
    int r;
    int d;
    int id;
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

            sort(jobs_.begin(), jobs_.end(), [](const Job& left, const Job& right) -> bool {
                if (left.d != right.d) {
                    return left.d < right.d;
                }
                if (left.r != right.r) {
                    return left.r < right.r;
                }
                if (left.inc != right.inc) {
                    return left.inc < right.inc;
                }
                return left.id < right.id;
            });

            std::vector<std::pair<int, std::pair<ll, int>>> dp;
            std::vector<int> prev(jobs_.size() + 1);

            for (int i = 0; i < jobs_.size(); ++i) {
                ll best = 0;
                int best_ind = -1;
                if (i > 0) {
                    auto it = upper_bound(dp.begin(), dp.end(), std::make_pair(jobs_[i].r, std::make_pair(INF, int(jobs_.size()))));
                    if (it != dp.begin()) {
                        --it;
                        best = it->second.first;
                        best_ind = it->second.second;
                    }
                }
                std::pair<int, std::pair<ll, int>> curr = {jobs_[i].d, std::make_pair(best + jobs_[i].inc, jobs_[i].id)};
                prev[jobs_[i].id] = best_ind;
                while (!dp.empty() && dp.back().first == curr.first && dp.back().second.first < curr.second.first) {
                    dp.pop_back();
                }
                if (dp.empty() || curr.second.first > dp.back().second.first) {
                    dp.push_back(curr);
                }
            }

            ll ans = dp.back().second.first;

            int ind = dp.back().second.second;
            
            std::vector<int> indexes;
            while (ind != -1) {
                indexes.push_back(ind);
                ind = prev[ind];
            }

            reverse(indexes.begin(), indexes.end());

            return {ans, indexes};
        }
};  

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    int n;
    std::cin >> n;
    std::vector<Job> a(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i].r >> a[i].d >> a[i].inc;
        a[i].id = i + 1;
    }
    auto [ans, indexes] = IncomeMaximizer(a).Calc();
    std::cout << ans << ' ' << indexes.size() << '\n';
    for (auto elem : indexes) {
        std::cout << elem << ' ';
    }
    std::cout << '\n';
}