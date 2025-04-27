#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <vector>

using ll = long long;

struct Job{
    int d;
    int p;
    int id;
};

bool operator <(const Job& left, const Job& right) {
    if (left.d != right.d) {
        return left.d < right.d;
    }
    if (left.p != right.p) {
        return left.p < right.p;
    }
    return left.id < right.id;
}

namespace CONSTANTS {
    const ll INF = 2'000'000'000'000'000'000;
};

class TardinessMinimizer{
    private:
        std::vector<Job> jobs_;
        std::map<std::pair<std::vector<Job>, int>, std::vector<Job>> mp_;

        static int GetJStar(const std::vector<Job>& jobs) {
            int best = 0;
            for (int i = 0; i < jobs.size(); ++i) {
                if (jobs[i].p >= jobs[best].p) {
                    best = i;
                }
            }
            return best;
        }

        static std::vector<Job> BeforeInd(const std::vector<Job>& jobs, int ind) {
            if (ind == 0) {
                return {};
            }
            return std::vector<Job>(jobs.begin(), jobs.begin() + ind);
        }

        static std::vector<Job> AfterInd(const std::vector<Job>& jobs, int ind) {
            if (ind == jobs.size()) {
                return {};
            }
            return std::vector<Job>(jobs.begin() + ind, jobs.end());
        }

        static ll FindCost(const std::vector<Job>& jobs, int t) {
            ll res = 0;
            for (auto elem : jobs) {
                res += std::max(0, elem.p + t - elem.d);
                t += elem.p;
            }
            return res;
        }

        std::vector<Job> RecursiveSchedule(std::vector<Job> jobs, int t) {
            if (jobs.empty()) {
                return {};
            }
            if (jobs.size() == 1) {
                return jobs;
            }
            if (mp_.find({jobs, t}) != mp_.end()) {
                return mp_[{jobs, t}];
            }
            int j_star = GetJStar(jobs);
            std::vector<Job> left = BeforeInd(jobs, j_star);
            std::vector<Job> right = AfterInd(jobs, j_star + 1);

            ll left_t = 0;
            for (int i = 0; i <= j_star; ++i) {
                left_t += jobs[i].p;
            }

            std::vector<Job> best;
            ll res = CONSTANTS::INF;
            int sv = right.size();
            for (int k = 0; k <= sv; ++k) {
                std::vector<Job> curr = RecursiveSchedule(left, t);
                curr.push_back(jobs[j_star]);
                std::vector<Job> tmp = RecursiveSchedule(right, left_t);
                if (!tmp.empty()) {
                    curr.insert(curr.end(), tmp.begin(), tmp.end());
                }
                if (j_star + k + 1 < jobs.size()) {
                    left.push_back(right[0]);
                    right.erase(right.begin());
                    left_t += jobs[j_star + k + 1].p;
                }

                ll curr_cost = FindCost(curr, t);

                if (best.empty() || curr_cost < res) {
                    std::swap(best, curr);
                    res = curr_cost;
                }
            }
            mp_[{jobs, t}] = best;
            return best;
        }
    public:
        TardinessMinimizer(std::vector<Job> jobs): jobs_(jobs) {}

        std::vector<Job> GetSchedule() {
            sort(jobs_.begin(), jobs_.end(), [](const Job& left, const Job& right) -> bool{
                if (left.d != right.d) {
                    return left.d < right.d;
                }
                if (left.p != right.p) {
                    return left.p < right.p;
                }
                return left.id < right.id;
            });
            return RecursiveSchedule(jobs_, 0);
        }
        ll FindCost(const std::vector<Job>& jobs) {
            return FindCost(jobs, 0);
        }
};

int main() {
    int n;
    std::cin >> n;
    std::vector<int> p(n);
    std::vector<int> d(n);
    for (auto& elem : p) {
        std::cin >> elem;
    }
    for (auto& elem : d) {
        std::cin >> elem;
    }
    std::vector<Job> a(n);
    for (int i = 0; i < n; ++i) {
        a[i] = Job{d[i], p[i], i};
    }
    TardinessMinimizer tm(a);
    std::vector<Job> schedule = tm.GetSchedule();
    std::cout << tm.FindCost(schedule) << '\n';
    // for (auto elem : schedule) {
    //     std::cout << elem.id << ' ';
    // }
    // std::cout << '\n';
}