#include <iostream>
#include <vector>
#include <stack>
#include <time.h>
#include <cmath>
#include <string>
#include <chrono>
#include <iomanip>

template<typename T>
class ScapeGoatTree{
public:
    struct Node{
        Node* left;
        Node* right;
        Node* brother;
        T key;
        int sz;

        Node(Node* left, Node* right, Node* brother, const T& key, int sz): left(left), right(right), brother(brother), key(key), sz(sz) {}
    };

private:

    double alpha_;

    Node* root_;

    int max_count_;

    template<typename S>
    void clear(std::stack<S>& st) {
        while (!st.empty()) {
            st.pop();
        }
    }


    std::stack<Node*> InsertKey(T key) {
        Node* curr = root_;

        std::stack<Node*> result;

        if (root_ == nullptr) {
            root_ = new Node(nullptr, nullptr, nullptr, key, 1);
            result.push(root_);
            return result;
        }

        while (true) {
            result.push(curr);
            ++curr->sz;

            if (curr->key == key) {
                clear(result);
                return result;
            }

            if (curr->key < key) {
                if (curr->right == nullptr) {
                    curr->right = new Node(nullptr, nullptr, curr->left, key, 1);
                    curr->right->brother = curr->left;
                    break;
                }
                curr = curr->right;
                continue;
            }

            if (curr->left == nullptr) {
                curr->left = new Node(nullptr, nullptr, curr->right, key, 1);
                result.push(curr->left);
                break;
            }
            curr = curr->left;
        }

        return result;        
    }

    std::pair<Node*, Node*> FindScapegoat(std::stack<Node*>& st) { // scapegoat, parent
        int height = 1;

        while (!st.empty()) {

            if (st.top()->sz != 0) {

                int max_h = log(st.top()->sz) / log(1.0 / alpha_);

                if (height > max_h) {
                    Node* res = st.top();
                    Node* par = nullptr;
                    st.pop();
                    if (!st.empty()) {
                        par = st.top();
                    }
                    return {res, par};
                }
            }

            st.pop();

            ++height;
        }

        return {root_, nullptr};
    }

    void InOrderDFS(Node* curr, std::vector<Node*>& order) {
        if (curr == nullptr) {
            return;
        }
        if (curr->left != nullptr) {
            InOrderDFS(curr->left, order);
        }
        order.push_back(curr);
        if (curr->right != nullptr) {
            InOrderDFS(curr->right, order);
        }
        curr->left = nullptr;
        curr->right = nullptr;
        curr->brother = nullptr;
        curr->sz = 0;
    }

    Node* GenerateTree(int left, int right, std::vector<Node*>& order) {
        if (left == right) {
            order[left]->sz = 1;
            return order[left];
        }
        if (left + 1 == right) {
            order[left]->right = order[right];
            order[right]->sz = 1;
            order[left]->sz = 2;
            return order[left];
        }

        int mid = (left + right) / 2;
        Node* left_nd = GenerateTree(left, mid - 1, order);
        Node* right_nd = GenerateTree(mid + 1, right, order);
        left_nd->brother = right_nd;
        right_nd->brother = left_nd;

        order[mid]->left = left_nd;
        order[mid]->right = right_nd;
        order[mid]->sz = left_nd->sz + right_nd->sz + 1;

        return order[mid];
    }

    Node* EraseMin(Node* curr) {
        Node* last = nullptr;
        while (curr->left != nullptr) {
            --curr->sz;
            last = curr;
            curr = curr->left;
        }
        if (last != nullptr) {
            last->left = curr->right;
        }
        return curr;
    }

    std::stack<Node*> FixAfterDeleteCommon(std::stack<Node*>& result, Node* curr, T key) {
        if (curr->left == nullptr && curr->right == nullptr) {
            if (result.top()->key > key) {
                result.top()->left = nullptr;
            } else {
                result.top()->right = nullptr;
            }
            --result.top()->sz;
            delete curr;
            return result;
        }

        if (curr->left == nullptr) {
            if (result.top()->key > key) {
                result.top()->left = curr->right;
            } else {
                result.top()->right = curr->right;
            }
            --result.top()->sz;
            result.push(curr->right);
            delete curr;
            return result;
        }

        if (curr->right == nullptr) {
            if (result.top()->key > key) {
                result.top()->left = curr->left;
            } else {
                result.top()->right = curr->left;
            }
            --result.top()->sz;
            result.push(curr->left);
            delete curr;
            return result;
        }

        Node* mn = EraseMin(curr->right);

        if (result.top()->key > key) {
            result.top()->left = mn;
        } else {
            result.top()->right = mn;
        }

        mn->left = curr->left;
        if (curr->right != mn) {
            mn->right = curr->right;
        }
        
        mn->sz = 1 + (mn->left == nullptr ? 0 : mn->left->sz) + (mn->right == nullptr ? 0 : mn->right->sz);

        result.push(mn);

        delete curr;
        return result;
    }

    std::stack<Node*> FixAfterDeleteRoot() {
        std::stack<Node*> result;
        if (root_->left == nullptr && root_->right == nullptr) {
            delete root_;
            root_ = nullptr;
            return result;
        }

        if (root_->left == nullptr) {
            Node* curr = root_->right;
            delete root_;
            root_ = curr;
            result.push(curr);
            return result;
        }

        if (root_->right == nullptr) {
            Node* curr = root_->left;
            delete root_;
            root_ = curr;
            result.push(curr);
            return result;
        }

        Node* mn = EraseMin(root_->right);
        
        mn->left = root_->left;
        if (root_->right != mn) {
            mn->right = root_->right;
        }

        mn->sz = 1 + (mn->left == nullptr ? 0 : mn->left->sz) + (mn->right == nullptr ? 0 : mn->right->sz);

        result.push(mn);

        delete root_;
        root_ = mn;

        return result;
    }

    std::stack<Node*> DeleteKey(const T& key) {
        Node* curr = root_;
        std::stack<Node*> result;

        if (root_ == nullptr) {
            return result;
        }

        while (true) {
            result.push(curr);
            --curr->sz;
            if (curr->key == key) {
                break;
            }

            if (curr->key < key) {
                if (curr->right == nullptr) {
                    clear(result);
                    return result;
                }
                curr = curr->right;
                continue;
            }

            if (curr->left == nullptr) {
                clear(result);
                return result;
            }

            curr = curr->left;
        }

        curr = result.top();
        result.pop();

        if (result.empty()) {
            return FixAfterDeleteRoot();
        }

        return FixAfterDeleteCommon(result, curr, key);

    }

    void DeleteTree(Node* curr) {
        if (curr == nullptr) {
            return;
        }
        if (curr->left != nullptr) {
            DeleteTree(curr->left);
        }
        if (curr->right != nullptr) {
            DeleteTree(curr->right);
        }
        delete curr;
    }

public:
    Node* Search(const T& key) {
        Node* curr = root_;

        while (curr != nullptr && curr->key != key) {
            if (curr->key < key) {
                curr = curr->right;
            } else {
                curr = curr->left;
            }
        }

        if (curr == nullptr) {
            return nullptr;
        }

        return curr;
    }

    bool Insert(const T& key) {
        std::stack<Node*> st = InsertKey(key);

        if (st.empty() && root_ != nullptr) {
            return false;
        }

        if (root_->sz == 0) {
            return true;
        }

        int max_h = log(root_->sz) / log(1.0 / alpha_);

        if (st.size() > max_h) {
            std::pair<Node*, Node*> scapegoat = FindScapegoat(st);
            bool flag = false;
            if (scapegoat.second != nullptr && scapegoat.first->key > scapegoat.second->key) {
                flag = true;
            }
            std::vector<Node*> order;
            InOrderDFS(scapegoat.first, order);
            scapegoat.first = GenerateTree(0, order.size() - 1, order);
            if (scapegoat.second == nullptr) {
                root_ = scapegoat.first;
            } else {
                if (flag) {
                    scapegoat.second->right = scapegoat.first;
                } else {
                    scapegoat.second->left = scapegoat.first;
                }
            }
        }

        max_count_ = std::max(8, 2 * root_->sz);

        return true;
    }

    bool Delete(const T& key) {
        if (root_ == nullptr) {
            return false;
        }

        std::stack<Node*> st = DeleteKey(key);

        if (st.empty() && root_ != nullptr) {
            return false;
        }

        if (root_ == nullptr) {
            return true;
        }
 
        if (root_->sz < int(max_count_ * alpha_)) {
            std::vector<Node*> order;
            InOrderDFS(root_, order);
            root_ = GenerateTree(0, order.size() - 1, order);
            max_count_ = std::max(8, 2 * root_->sz);
        }

        return true;
    }

    ScapeGoatTree(double alpha): alpha_(alpha), max_count_(8), root_(nullptr) {
    }


    ~ScapeGoatTree() {
        DeleteTree(root_);
    }
};

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    char* endptr;
    double alpha = strtod(argv[1], &endptr);
    ScapeGoatTree<int> sgt(alpha);

    int q;
    std::cin >> q;
    int64_t ans = 0;
    bool flag = false;
    std::chrono::_V2::system_clock::time_point start_time;
    while (q--) {
        std::string s;
        std::cin >> s;
        if (s == "insert") {
            int x;
            std::cin >> x;
            sgt.Insert(x);
        }
        if (s == "delete") {
            if (!flag) {
                start_time = std::chrono::high_resolution_clock::now();
                flag = true;
            }
            int x;
            std::cin >> x;
            sgt.Delete(x);
        }
        if (s == "search") {
            int x;
            std::cin >> x;
            sgt.Search(x);
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    ans = diff.count();
    std::cout << alpha << ' ' << ans << '\n';
}