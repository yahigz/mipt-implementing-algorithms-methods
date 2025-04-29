#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

struct Job{
    int p;
    int d;
    int w;
    int id;
};

using ll = long long;

namespace CONSTANTS {
    const ll INF = 2'000'000'000'000'000'000;
}

class WeightedTardinessMinimizer{
    private:
        std::vector<Job> jobs_;

        std::vector<ll> GetSuffixSum() const {
            std::vector<ll> suff(jobs_.size());
            suff.back() = jobs_.back().p;
            for (int i = suff.size() - 2; i >= 0; --i) {
                suff[i] = suff[i + 1] + jobs_[i].p;
            } 
            return suff;
        }

        ll GetSumFromIndToEnd(int ind, const std::vector<ll>& suff) const {
            return (ind == suff.size() ? 0LL : suff[ind]);
        }

        struct Node{
            Node* left;
            Job job;
            Node(Node* left, Job job): left(left), job(job) {}
        };

        struct List{
            Node* head;
            Node* tail;

            List(): head(nullptr), tail(nullptr) {}

            List(Node* root): head(root), tail(root) {}

            void PushBack(Node* to_push) {
                to_push->left = tail;
                tail = to_push;
                if (head == nullptr) {
                    head = to_push;
                }
            }
            
            void PushFront(Node* to_push) {
                if (head == nullptr) {
                    head = to_push;
                    tail = to_push;
                    return;
                }
                head->left = to_push;
                head = to_push;
            }
        };

        void GeneratePath(int j, int t, List& lst, std::vector<std::vector<std::pair<int, ll>>>& p) {
            if (p[j][t] == std::make_pair(-1, -1LL)) {
                lst = List(new Node(nullptr, jobs_[j]));
                return;
            }
            auto [next_p, next_t] = p[j][t];
            GeneratePath(next_p, next_t, lst, p);
            if (next_t == t) {
                lst.PushBack(new Node(nullptr, jobs_[j]));
            } else {
                lst.PushFront(new Node(nullptr, jobs_[j]));
            }
        }

    public:
        WeightedTardinessMinimizer(std::vector<Job> jobs): jobs_(jobs) {}

        std::pair<std::vector<Job>, ll> GetSchedule() {
            sort(jobs_.begin(), jobs_.end(), [](const Job& left, const Job& right) -> bool {
                if (left.d != right.d) {
                    return left.d < right.d;
                }
                if (left.p != right.p) {
                    return left.p < right.p;
                }
                if (left.w != right.w) {
                    return left.w < right.w;
                }
                return left.id < right.id;
            });
            
            reverse(jobs_.begin(), jobs_.end());

            std::vector<ll> suff = GetSuffixSum();
            std::vector<List> sch(suff[0] + 1);
            std::vector<ll> opt(suff[0] + 1, 0);
            std::vector<std::vector<std::pair<int, ll>>> p(jobs_.size(), std::vector<std::pair<int, ll>>(suff[0] + 1, {-1, -1}));
            for (int t = 0; t <= GetSumFromIndToEnd(1, suff); ++t) {
                if (t + jobs_[0].p - jobs_[0].d <= 0) {
                    opt[t] = jobs_[0].w;
                }  else {
                    opt[t] = 0;
                }
            }
            for (int j = 1; j < jobs_.size(); ++j) {
                std::vector<ll> new_opt(suff[0] + 1, 0);
                for (int t = 0; t <= suff[0] - (GetSumFromIndToEnd(0, suff) - GetSumFromIndToEnd(j + 1, suff)); ++t) {
                    ll opt_to_begin, opt_to_end;

                    if (t + jobs_[j].p - jobs_[j].d <= 0) {
                        opt_to_begin = jobs_[j].w + opt[t + jobs_[j].p];
                    } else {
                        opt_to_begin = opt[t + jobs_[j].p];
                    }

                    if (t + GetSumFromIndToEnd(0, suff) - GetSumFromIndToEnd(j + 1, suff) - jobs_[j].d <= 0) {
                        opt_to_end = opt[t] + jobs_[j].w;
                    } else {
                        opt_to_end = opt[t];
                    }

                    if (opt_to_begin < opt_to_end) {
                        p[j][t] = {j - 1, t};
                        new_opt[t] = opt_to_end;
                    } else {
                        p[j][t] = {j - 1, t + jobs_[j].p};
                        new_opt[t] = opt_to_begin;
                    }
                }
                std::swap(new_opt, opt);
            }

            std::vector<Job> schedule;
            ll nontardiness = opt[0];

            List lst;
            GeneratePath(jobs_.size() - 1, 0, lst, p);
            Node* curr = lst.tail;
            while (curr != nullptr) {
                Node* sv = curr;
                schedule.push_back(curr->job);
                curr = curr->left;
                delete sv;
            }
            reverse(schedule.begin(), schedule.end());
            ll sum_w = 0;
            for (auto elem : jobs_) {
                sum_w += elem.w;
            }
            ll tardiness = sum_w - nontardiness;
            return {schedule, tardiness};
        }
};

// #define KNAPSACK_SOLVER

int main() {
    int n;
    std::cin >> n;
    std::vector<Job> a(n);
    #ifdef KNAPSACK_SOLVER
        int m;
        std::cin >> m;
        for (int i = 0; i < n; ++i) {
            std::cin >> a[i].p;
        }
        for (int i = 0; i < n; ++i) {
            std::cin >> a[i].w;
        }
        for (int i = 0; i < n; ++i) {
            a[i].d = m;
            a[i].id = i;
        }
        auto [sch, ans] = WeightedTardinessMinimizer(a).GetSchedule();  

        std::cout << ans << '\n';
    #else
        for (int i = 0; i < n; ++i) {
            std::cin >> a[i].p;
        }
        for (int i = 0; i < n; ++i) {
            std::cin >> a[i].d;
        }
        for (int i = 0; i < n; ++i) {
            std::cin >> a[i].w;
            a[i].id = i;
        }
        auto [sch, ans] = WeightedTardinessMinimizer(a).GetSchedule();

        std::cout << ans << '\n';  
        for (auto elem : sch) {
            std::cout << elem.id << ' ';
        }
        std::cout << '\n';
    #endif
}
