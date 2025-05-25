#include <iostream>
#include <vector>
#include <cstdio>
#include <iomanip>

class BSTOptimizer {
    using ll = long long;
private:
    std::vector<double> success_;
    std::vector<double> fail_;

    std::vector<int> preorder_;

    template<typename T> 
    static T get_sum(int i, int j, const std::vector<T>& pref) {
        return (i == 0 ? pref[j] : pref[j] - pref[i - 1]);
    }

    static void gen_graph(int par, int i, int j, const std::vector<std::vector<int>>& p, std::vector<std::vector<int>>& gr) {
        int to = p[i][j];

        if (p[i][j] == -1) {
            to = i;
        }

        if (par != -1) {
            gr[par].push_back(to);
        }

        if (to != i) {
            gen_graph(to, i, to - 1, p, gr);
        }
        if (to != j) {
            gen_graph(to, to + 1, j, p, gr);
        }
    }

    static void preorder_dfs(int v, const std::vector<std::vector<int>>& gr, std::vector<int>& path) {
        path.push_back(v);
        for (auto to : gr[v]) {
            preorder_dfs(to, gr, path);
        }
    }

public:
    BSTOptimizer(const std::vector<double>& success, const std::vector<double>& fail): success_(success), fail_(fail) {}

    double GetBSTVal() {
        const ll INF = 1e9;
        int n = success_.size();
        std::vector<std::vector<double>> dp(n, std::vector<double>(n, INF));
        std::vector<std::vector<int>> p(n, std::vector<int>(n, -1));
        std::vector<double> pref_success(n), pref_fail(n + 1);
        
        pref_success[0] = success_[0];
        pref_fail[0] = fail_[0];
        for (int i = 1; i < n; ++i) {
            pref_success[i] = pref_success[i - 1] + success_[i];
            pref_fail[i] = pref_fail[i - 1] + fail_[i];
        }
        pref_fail[n] = pref_fail[n - 1] + fail_[n];

        for (int i = 0; i < n; ++i) {
            dp[i][i] = success_[i];
            dp[i][i] += fail_[i] + fail_[i + 1];
        }

        for (int len = 2; len <= n; ++len) {
            for (int i = 0; i < n && i + len - 1 < n; ++i) {
                for (int j = i; j < i + len; ++j) {
                    int k = i + len - 1;
                    // [i ... j) j (j... k]

                    double left_dp = (i != j ? dp[i][j - 1] : 0);
                    double right_dp = (k != j ? dp[j + 1][k] : 0);

                    double left_sum = (i != j ? get_sum(i, j - 1, pref_success) + get_sum(i, j, pref_fail) : 0);
                    double right_sum = (k != j ? get_sum(j + 1, k, pref_success) + get_sum(j + 1, k + 1, pref_fail) : 0);

                    double result = left_dp + right_dp + left_sum + right_sum + success_[j];

                    if (i == j) {
                        result += fail_[i];
                    }

                    if (j == k) {
                        result += fail_[k + 1];
                    }

                    if (result < dp[i][k]) {
                        dp[i][k] = result;
                        p[i][k] = j;
                    }
                }
            }
        }

        std::vector<std::vector<int>> gr(n);
        gen_graph(-1, 0, n - 1, p, gr);
        preorder_dfs(p[0][n - 1] == -1 ? 0 : p[0][n - 1], gr, preorder_);
 
        return dp[0][n - 1];
    }

    std::vector<int> GetBST() const {
        return preorder_;
    }
};

int main() {
    int n;
    std::cin >> n;
    std::vector<double> p(n);
    std::vector<double> q(n + 1);
    for (auto& elem : p) {
        std::cin >> elem;
    }
    for (auto& elem : q) {
        std::cin >> elem;
    }
    BSTOptimizer bst(p, q);
    std::cout << std::fixed << std::setprecision(10) << bst.GetBSTVal() << '\n';
}