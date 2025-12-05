#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>

template <typename T>
struct LazySumOp {
    T operator()(T a, T b) const { return a + b; }
    T neutral() const { return 0; }

    T apply(T currentVal, T lazyVal, int len) const { 
        return currentVal + (lazyVal * len); 
    }

    T compose(T currentLazy, T newLazy) const { 
        return currentLazy + newLazy; 
    }

    T neutralLazy() const { return 0; }
};

template <typename T>
struct LazyMinAddOp {
    T operator()(T a, T b) const { 
        return std::min(a, b); 
    }

    T neutral() const { 
        return std::numeric_limits<T>::max(); 
    }

    T apply(T currentVal, T lazyVal, int /*len*/) const { 
        if (currentVal == neutral()) return currentVal;
        return currentVal + lazyVal; 
    }

    T compose(T currentLazy, T newLazy) const { 
        return currentLazy + newLazy; 
    }

    T neutralLazy() const { return 0; }
};

template <typename T>
struct LazyMaxAddOp {
    T operator()(T a, T b) const { 
        return std::max(a, b); 
    }

    T neutral() const { 
        return std::numeric_limits<T>::lowest(); 
    }

    T apply(T currentVal, T lazyVal, int /*len*/) const { 
        if (currentVal == neutral()) return currentVal;
        return currentVal + lazyVal; 
    }

    T compose(T currentLazy, T newLazy) const { 
        return currentLazy + newLazy; 
    }

    T neutralLazy() const { return 0; }
};


template <typename T, typename Operation>
class CSegmentTree {
private:
    std::vector<T> m_Tree;
    std::vector<T> m_Lazy;
    int m_nSize;
    Operation m_Op;

    void Push(int node, int start, int end) {
        if (m_Lazy[node] == m_Op.neutralLazy()) return;
        if (start != end) { // Si no es hoja, propagamos a hijos
            int mid = (start + end) / 2;
            int left = 2 * node;
            int right = 2 * node + 1;

            m_Tree[left] = m_Op.apply(m_Tree[left], m_Lazy[node], mid - start + 1);
            m_Tree[right] = m_Op.apply(m_Tree[right], m_Lazy[node], end - mid);

            m_Lazy[left] = m_Op.compose(m_Lazy[left], m_Lazy[node]);
            m_Lazy[right] = m_Op.compose(m_Lazy[right], m_Lazy[node]);
        }

        m_Lazy[node] = m_Op.neutralLazy();
    }

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

    void Update(int node, int start, int end, int l, int r, T val) {
        Push(node, start, end);
        if (start > end || start > r || end < l) return;

        if (start >= l && end <= r) {
            m_Tree[node] = m_Op.apply(m_Tree[node], val, end - start + 1);
            m_Lazy[node] = m_Op.compose(m_Lazy[node], val);
            return;
        }
        int mid = (start + end) / 2;
        Update(2 * node, start, mid, l, r, val);
        Update(2 * node + 1, mid + 1, end, l, r, val);
        m_Tree[node] = m_Op(m_Tree[2 * node], m_Tree[2 * node + 1]);
    }

    T Query(int node, int start, int end, int l, int r) {
        Push(node, start, end);
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
        m_Lazy.assign(4 * m_nSize, m_Op.neutralLazy()); 
        Build(data, 1, 0, m_nSize - 1);
    }

    void Update(int left, int right, T value) {
        Update(1, 0, m_nSize - 1, left, right, value);
    }

    void Update(int index, T value) {
        Update(1, 0, m_nSize - 1, index, index, value);
    }

    T Query(int left, int right) {
        if (left > right) return m_Op.neutral();
        return Query(1, 0, m_nSize - 1, left, right);
    }
};