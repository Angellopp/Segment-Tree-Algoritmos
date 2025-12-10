#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>
#include <string>
#include <sstream>
#include <cmath>

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
        if (start != end) {
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

    friend std::ostream& operator<<(std::ostream& os, const CSegmentTree& st) {
        os << "\n-------------------------------------------\n";
        os << "Formato: [VALOR] (Rango) {Lazy}\n\n";

        auto print_recursive = [&](auto&& self, int node, int start, int end, int depth, bool isLeft) -> void {
            if (node >= (int)st.m_Tree.size()) return;

            // DERECHO 
            if (start != end) {
                int mid = (start + end) / 2;
                self(self, 2 * node + 1, mid + 1, end, depth + 1, false);
            }

            // NODO ACTUAL
            os << std::string(depth * 6, ' '); 

            if (depth > 0) {
                if (isLeft) { os << char(192); } 
                else { os << char(218); }
                os << char(196) << char(196) << char(196) << " ";
            } 
            
            // valores
            T val = st.m_Tree[node];
            T lazy = st.m_Lazy[node];
            bool hasLazy = (lazy != st.m_Op.neutralLazy());

            // Imprimir Valor
            if (val == st.m_Op.neutral()) os << "[N]"; // Neutro
            else os << "[" << val << "]";

            // Imprimir Rango
            os << " (" << start << "-" << end << ")";

            // Imprimir Lazy (Solo si existe)
            if (hasLazy) {
                os << " {Lazy: " << lazy << "}"; //
            }
            os << "\n";

            // 3. Imprimir hijo IZQUIERDO (abajo)
            if (start != end) {
                int mid = (start + end) / 2;
                self(self, 2 * node, start, mid, depth + 1, true);
            }
        };

        print_recursive(print_recursive, 1, 0, st.m_nSize - 1, 0, false);
        
        os << "-------------------------------------------\n";
        return os;
    }
};