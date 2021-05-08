// heap.h (Binary Heap)

#include <vector>
#include <stdexcept>

template <typename T>     // assume that T has the < operator
class Heap {
public:
    bool is_empty() const
    {
        return _data.size() == 0;
    }

    const T & find_min() const
    {
        if (is_empty()) {
            throw std::runtime_error("Empty heap; Heap::find_min failed.");
        }
        return _data[0];
    }

    T extract_min()
    {
        T result = find_min();
        remove(0);
        return result;
    }

    int insert(const T & object)
    {
        _data.push_back(object);
        sift_up(_data.size() - 1);
        return _data.size() - 1;
    }

protected:                         // accessible only for derived classes
    void remove(int index)
    {
        ensure_is_valid_index(index);
        swap(_data[index], _data[_data.size() - 1]);
        _data.pop_back();
        sift_up(index);
        sift_down(index);
    }

    void decrease_key(int index)
    {
        ensure_is_valid_index(index);
        sift_up(index);
    }

    virtual void swap(T & a, T & b)             // virtual functions can be
    {                                           // overridden by derived classes
        std::swap(a, b);
    }

    T & get_object(int index)
    {
        ensure_is_valid_index(index);
        return _data[index];
    }

private:
    void  ensure_is_valid_index(int index)
    {
        if (index >= static_cast<int>(_data.size()) or index < 0)
            throw std::runtime_error("Index error in heap operation");
    }

    static int parent(int index)          // do not call with index==0!
    {
        return (index - 1) / 2;
    }

    static int left(int index)            // left child may not exist!
    {
        return (2 * index) + 1;
    }

    static int right(int index)           // right child may not exist!
    {
        return (2 * index) + 2;
    }

    void sift_up(int index)
    {
        while ((index > 0) and (_data[index] < _data[parent(index)])) {
            swap(_data[index], _data[parent(index)]);
            index = parent(index);
        }
    }

    void sift_down(int index)
    {
        int smallest = index;
        while (true) {
            if ((left(index) < static_cast<int>(_data.size())) and
                (_data[left(index)] < _data[smallest]))
            {
                smallest = left(index);
            }
            if ((right(index) < static_cast<int>(_data.size())) and
                (_data[right(index)] < _data[smallest]))
            {
                smallest = right(index);
            }
            if (index == smallest) return;
            swap(_data[smallest], _data[index]);
            index = smallest;
        }
    }

    std::vector<T> _data;       // holds the objects in heap order
};

