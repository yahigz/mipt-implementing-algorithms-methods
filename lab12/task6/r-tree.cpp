#include <algorithm>
#include <iostream>
#include <vector>
#include <cassert>

class RTree{
private:

    struct Node{
        const int INF = 2e9;
        int min_x, max_x, min_y, max_y, height;
        std::vector<Node*> children;
        Node* parent;

        bool is_leaf;

        Node(int min_x, int max_x, int min_y, int max_y, Node* parent, bool is_leaf, int height): min_x(min_x), max_x(max_x), min_y(min_y),
                                                                                    is_leaf(is_leaf), max_y(max_y), parent(parent), height(height) {}

        int CalcDelta(Node* another) {
            return std::abs(min_x - another->min_x) + std::abs(max_x - another->max_x) + std::abs(min_y - another->min_y) + std::abs(max_y - another->max_y);
        }

        void Update(Node* child) {
            min_x = std::min(min_x, child->min_x);
            max_x = std::max(max_x, child->max_x);
            min_y = std::min(min_y, child->min_y);
            max_y = std::max(max_y, child->max_y);
        }

        bool Contains(int mnx, int mxx, int mny, int mxy) {
            return (min_x <= mnx && mxx <= max_x && min_y <= mny && mxy <= max_y);
        }

        bool Equiv(int mnx, int mxx, int mny, int mxy) {
            return min_x == mnx && max_x == mxx && min_y == mny && max_y == mxy;
        }
        
        void Reset() {
            min_x = INF;
            max_x = -INF;
            min_y = INF;
            max_y = -INF;
        }
    };  

    Node* root_ = nullptr;

    int max_entries_;                                                                                                                       
    int min_entries_;

    const std::pair<Node*, Node*> SUCCESS = {nullptr, nullptr};

    std::pair<Node*, Node*> LinearSplit(Node* curr) {
        std::vector<Node*> v;
        std::swap(v, curr->children);
        int argminx = 0;
        int argmaxx = 0;
        int argminy = 0;
        int argmaxy = 0;
        for (int i = 0; i < v.size(); ++i) {
            if (v[i]->min_x < v[argminx]->min_x) {
                argminx = i;
            }
            if (v[i]->max_x > v[argmaxx]->max_x) {
                argmaxx = i;
            }
            if (v[i]->min_y < v[argminy]->min_y) {
                argminy = i;
            }
            if (v[i]->max_y > v[argmaxy]->max_y) {
                argmaxy = i;
            }
        }

        int seed1 = argminx, seed2 = argmaxx;

        int res = v[seed1]->CalcDelta(v[seed2]);
        
        std::vector<int> ptr = {argminx, argminy, argmaxx, argmaxy};
        
        for (int i = 0; i < 4; ++i) {
            for (int j = i + 1; j < 4; ++j) {
                if (res < v[ptr[i]]->CalcDelta(v[ptr[j]])) {
                    seed1 = ptr[i];
                    seed2 = ptr[j];
                    res = v[ptr[i]]->CalcDelta(v[ptr[j]]);
                }
            }
        }

        std::pair<Node*, Node*> result = {new Node(*v[seed1]), new Node(*v[seed2])};
        result.first->is_leaf = curr->is_leaf;
        result.second->is_leaf = curr->is_leaf;
        result.first->children = {v[seed1]};
        v[seed1]->parent = result.first;
        result.first->height = curr->height;
        result.second->children = {v[seed2]};
        v[seed2]->parent = result.second;
        result.second->height = curr->height;

        for (int i = 0; i < v.size(); ++i) {
            if (i == seed1 || i == seed2) {
                continue;
            }
            if (v[seed1]->CalcDelta(v[i]) < v[seed2]->CalcDelta(v[i])) {
                result.first->children.push_back(v[i]);
                result.first->Update(v[i]);
                v[i]->parent = result.first;
            } else {
                result.second->children.push_back(v[i]);
                result.second->Update(v[i]);
                v[i]->parent = result.second;
            }
        }

        delete curr;
        return result;
    }

    
    std::pair<Node*, Node*> DoInsert(Node* curr, Node* to_insert) {
        if (curr->children.size() < max_entries_) {
            curr->children.push_back(to_insert);
            to_insert->parent = curr;
            curr->Update(to_insert);
            return SUCCESS;
        }
        curr->children.push_back(to_insert);
        curr->Update(to_insert);
        to_insert->parent = curr;
        return LinearSplit(curr);
    }

    std::pair<Node*, Node*> ChooseNode(Node* curr, Node* to_insert) {
        if (curr->height == to_insert->height + 1) {
            return DoInsert(curr, to_insert);
        }

        Node* best = curr->children[0];
        int pos = 0;
        
        for (int i = 1; i < curr->children.size(); ++i) {
            if (to_insert->CalcDelta(best) > to_insert->CalcDelta(curr->children[i])) {
                best = curr->children[i];
                pos = i;
            }
        }

        std::pair<Node*, Node*> lst = ChooseNode(best, to_insert);

        if (lst == SUCCESS) {
            curr->Update(to_insert);
            return SUCCESS;
        }

        if (curr->children.size() < max_entries_) {
            curr->Update(lst.first);
            curr->Update(lst.second);
            curr->children[pos] = lst.first;
            lst.first->height = curr->height - 1;
            curr->children.push_back(lst.second);
            lst.second->height = curr->height - 1;
            lst.first->parent = curr;
            lst.second->parent = curr;
            return SUCCESS;
        }

        curr->children[pos] = lst.first;
        lst.first->height = curr->height - 1;
        curr->children.push_back(lst.second);
        lst.second->height = curr->height - 1;

        return LinearSplit(curr);
    }

    bool SearchBrute(Node* curr, int min_x, int max_x, int min_y, int max_y, Node*& result) {
        if (curr->is_leaf) {
            for (auto to : curr->children) {
                if (to->Equiv(min_x, max_x, min_y, max_y)) {
                    result = to;
                    return true;
                }
            }
            return false;
        }
        bool flag = false;
        for (auto to : curr->children) {
            if (to->Contains(min_x, max_x, min_y, max_y)) {
                flag |= SearchBrute(to, min_x, max_x, min_y, max_y, result);
                if (flag) {
                    return flag;
                }
            }
        }
        return flag;
    }

    std::vector<Node*> CondenseTree(Node* curr) {
        std::vector<Node*> to_reinsert;

        bool flag = true;
        while (curr != root_) {
            if (curr->children.size() < min_entries_) {
                std::vector<Node*> new_children;
                for (auto to : curr->parent->children) {
                    if (to == curr) {
                        continue;
                    }
                    new_children.push_back(to);
                }
                std::swap(curr->parent->children, new_children);
                curr->parent->Reset();
                for (auto elem : curr->parent->children) {
                    curr->parent->Update(elem);
                }
                to_reinsert.push_back(curr);
            } else {
                Node sv = *curr;
                curr->Reset();
                for (auto to : curr->children) {
                    curr->Update(to);
                }
                if (curr->Equiv(sv.min_x, sv.max_x, sv.min_y, sv.max_y)) {
                    flag = false;
                    break;
                }
            }
            curr = curr->parent;
        }

        if (flag && root_->children.size() < min_entries_) {
            if (root_->children.size() > 0) {
                to_reinsert.push_back(root_);
            }
            root_ = nullptr;
            return to_reinsert;
        }

        if (flag) {
            root_->Reset();
            for (auto to : root_->children) {
                root_->Update(to);
            }
        }

        return to_reinsert;
    } 

    void Reinsert(Node* curr) {
        std::pair<Node*, Node*> result = ChooseNode(root_, curr);
        if (result == SUCCESS) {
            root_->Update(curr);
            return;
        }
        root_ = new Node(*result.first);
        root_->Update(result.second);
        root_->parent = nullptr;
        root_->children.clear();
        root_->is_leaf = false;
        root_->height = result.first->height + 1;

        root_->children = {result.first, result.second};
        result.first->parent = root_;
        result.second->parent = root_;
    }

    void Clear(Node* curr) {
        for (auto to : curr->children) {
            Clear(to);
        }
        delete curr;
    }

public:

    RTree(int min_entries, int max_entries): min_entries_(min_entries), max_entries_(max_entries) {}

    void Insert(int min_x, int max_x, int min_y, int max_y) {
        if (Search(min_x, max_x, min_y, max_y)) {
            return;
        }

        Node* to_insert = new Node(min_x, max_x, min_y, max_y, nullptr, false, 0);

        if (root_ == nullptr) {
            root_ = new Node(min_x, max_x, min_y, max_y, nullptr, true, 1);
        }

        std::pair<Node*, Node*> result = ChooseNode(root_, to_insert);

        if (result == SUCCESS) {
            root_->Update(to_insert);
            return;
        }

        root_ = new Node(*result.first);
        root_->Update(result.second);
        root_->parent = nullptr;
        root_->children.clear();
        root_->is_leaf = false;
        root_->height = result.first->height + 1;   
        result.first->parent = root_;
        result.second->parent = root_;

        root_->children = {result.first, result.second};
    }

    bool Search(int min_x, int max_x, int min_y, int max_y) {
        if (root_ == nullptr) {
            return false;
        }
        Node* tmp = nullptr;
        return SearchBrute(root_, min_x, max_x, min_y, max_y, tmp);
    }
private:

    void InsertAll(Node* curr) {
        if (curr->is_leaf) {
            for (auto to : curr->children) {
                Insert(to->min_x, to->max_x, to->min_y, to->max_y);
            }
            delete curr;
            return;
        }
        for (auto to : curr->children) {
            InsertAll(to);
        }
        delete curr;
    }

public:    
    void Delete(int min_x, int max_x, int min_y, int max_y) {
        if (root_ == nullptr) {
            return;
        }
        Node* to_del = nullptr;
        if (!SearchBrute(root_, min_x, max_x, min_y, max_y, to_del)) {
            return;
        }

        std::vector<Node*> new_children;
        Node* curr = to_del->parent;
        for (auto to : curr->children) {
            if (to == to_del) {
                continue;
            }
            new_children.push_back(to);
        }

        delete to_del;

        std::swap(new_children, curr->children);
        curr->Reset();
        for (auto to : curr->children) {
            curr->Update(to);
        }

        if (curr->children.size() >= min_entries_) {
            return;
        }

        std::vector<Node*> to_reinsert = CondenseTree(curr);

        if (root_ == nullptr) {
            for (auto elem : to_reinsert) {
                InsertAll(elem);
            }
            return;
        }

        for (auto elem : to_reinsert) {
            if (elem->is_leaf) {
                for (auto to : elem->children) {
                    Insert(to->min_x, to->max_x, to->min_y, to->max_x);
                    delete to;
                }
                delete elem;
            } else {
                Reinsert(elem);
            }
        }
    }

    ~RTree() {
        if (root_ == nullptr) {
            return;
        }
        Clear(root_);
    }


};

int main() {
    RTree rt(4, 10);
    int n;
    std::cin >> n;
    while (n--) {
        std::string s;
        std::cin >> s;
        int mnx, mxx, mny, mxy;
        std::cin >> mnx >> mxx >> mny >> mxy;
        if (s == "insert") {
            rt.Insert(mnx, mxx, mny, mxy);
        }
        if (s == "search") {
            std::cout << rt.Search(mnx, mxx, mny, mxy) << '\n';
        }
        if (s == "delete") {
            rt.Delete(mnx, mxx, mny, mxy);
        }
    }
}