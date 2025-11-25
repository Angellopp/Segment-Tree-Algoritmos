#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>

template <typename T>
struct SumOp {
    T operator()(T a, T b) const { return a + b; }
    T neutral() const { return 0; }
};

template <typename T>
struct MinOp {
    T operator()(T a, T b) const { return std::min(a, b); }
    T neutral() const { return std::numeric_limits<T>::max(); } // Neutro es infinito
};

template <typename T>
struct MaxOp {
    T operator()(T a, T b) const { return std::max(a, b); }
    T neutral() const { return std::numeric_limits<T>::lowest(); } // Neutro es -infinito
};


template <typename T, typename Operation>
class CSegmentTree {
private:
    std::vector<T> m_Tree;
    int m_nSize;
    Operation m_Op;
    void Build(const std::vector<T>& data, int node, int start, int end) {
        if (start == end) {
            m_Tree[node] = data[start];
        } else {
            int mid = (start + end) / 2;
            Build(data, 2 * node, start, mid);
            Build(data, 2 * node + 1, mid + 1, end);
            m_Tree[node] = m_Op(m_Tree[2 * node], m_Tree[2 * node + 1]);
        }
    }

    void Update(int node, int start, int end, int idx, T val) {
        if (start == end) {
            m_Tree[node] = val;
        } else {
            int mid = (start + end) / 2;
            if (start <= idx && idx <= mid) {
                Update(2 * node, start, mid, idx, val);
            } else {
                Update(2 * node + 1, mid + 1, end, idx, val);
            }
            m_Tree[node] = m_Op(m_Tree[2 * node], m_Tree[2 * node + 1]);
        }
    }

    T Query(int node, int start, int end, int l, int r) {
        if (r < start || end < l) {
            return m_Op.neutral();
        }
        if (l <= start && end <= r) {
            return m_Tree[node];
        }
        int mid = (start + end) / 2;
        T p1 = Query(2 * node, start, mid, l, r);
        T p2 = Query(2 * node + 1, mid + 1, end, l, r);
        return m_Op(p1, p2);
    }

public:
    CSegmentTree(const std::vector<T>& data) {
        m_nSize = data.size();
        m_Tree.resize(4 * m_nSize); 
        Build(data, 1, 0, m_nSize - 1);
    }

    void Update(int index, T value) {
        if (index < 0 || index >= m_nSize) return;
        Update(1, 0, m_nSize - 1, index, value);
    }

    T Query(int left, int right) {
        if (left > right) return m_Op.neutral();
        return Query(1, 0, m_nSize - 1, left, right);
    }
};