namespace scaly {
namespace containers {

using namespace scaly::memory;

template<class T> struct Array : Object {
    size_t length;
    Vector<T>* vector;

    Array() : length(0), vector(nullptr) {}

    T* get_buffer() {
        if (this->vector == nullptr)
            return nullptr;
        return this->vector->data;
    }

    size_t get_length() {
        return this->length;
    }

    size_t get_capacity() {
        if (this->vector ==  nullptr)
            return 0;
        return this->vector->length;
    }

    void add(T item) {
        if (this->vector == nullptr || this->length == this->vector->length) {
            reallocate();
        }

        auto location = this->vector->data + this->length;
        *location = item;

        this->length++;
    }

    void add(const Vector<T>& items) {
        auto length = this->length + items.length;
        auto size = sizeof(T);
        if (this->vector == nullptr || length >= this->vector->length) {
            reallocate();
        }
        if (length >= this->vector->length) {
            auto own_page = Page::get(this);
            auto data = (T*)own_page->allocate_raw(length * size, alignof(T));
            auto bytes_to_copy = this->vector->length * size;
            if (this->vector->length > 0)
                memcpy(data, this->vector->data, bytes_to_copy);
            Page* old_exclusive_page = Page::get(this->vector->data);
            own_page->deallocate_exclusive_page(old_exclusive_page);
            this->vector->data = data;
            this->vector->length = length;
        }

        if (items.length > 0)
            memcpy(this->vector->data + this->length * size, items.data, items.length * size);
        this->length += items.length;
    }

    T* get(size_t i) {
        if (i >= this->length)
            return nullptr;

        return (*this)[i];
    }

    T* operator [](size_t i) {
        if (this->length < i + 1)
            return nullptr;
        return this->vector->data + i;
    }

    void reallocate() {
        auto own_page = Page::get(this);
        auto size = sizeof(T);
        size_t length;
        if (this->vector == nullptr) {
            Page* exclusive_page = own_page->allocate_exclusive_page();
            auto capacity = exclusive_page->get_capacity(alignof(T)) - sizeof(Vector<T>);
            length = capacity / size;
            this->vector = new(alignof(Vector<T>), own_page) Vector<T>((T*)nullptr, length);
            this->vector->data = (T*)exclusive_page->allocate_raw(capacity, alignof(T));             
        } else {
            length = this->vector->length * 2;
            auto data = (T*)own_page->allocate_raw(length * size, alignof(T));
            auto bytes_to_copy = this->vector->length * size;
            memcpy(data, this->vector->data, bytes_to_copy);
            Page* old_exclusive_page = Page::get(this->vector->data);
            own_page->deallocate_exclusive_page(old_exclusive_page);
            this->vector->data = data;
            this->vector->length = length;
        }
    }
};

}
}