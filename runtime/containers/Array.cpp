using namespace scaly::memory;

template<class T> struct Array : Object {
    size_t length;
    Vector<T>* vector;

    static Array<T>* create(Page* _rp) {
        return new(alignof(Array<T>), _rp) Array<T> {
            .length = 0,
            .vector = nullptr,
        };
    }

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

    void reallocate() {
        auto _own_page = Page::get(this);
        auto size = sizeof(T);
        size_t length;
        if (this->vector == nullptr) {
            Page& exclusive_page = *_own_page->allocate_exclusive_page();
            auto capacity = exclusive_page.get_capacity(alignof(T)) - sizeof(Vector<int>);
            length = capacity / size;
            this->vector = Vector<T>::create_without_buffer(_own_page, length);
            this->vector->data = (T*)exclusive_page.allocate_raw(capacity, alignof(T));             
        } else {
            length = this->vector->length * 2;
            auto oversized_page = _own_page->create_oversized_page(length * size);
            auto data = (T*)(oversized_page + 1);
            auto bytes_to_copy = this->vector->length * sizeof(T);
            memcpy(data, this->vector->data, bytes_to_copy);
            Page* old_exclusive_page = Page::get(this->vector->data);
            _own_page->deallocate_exclusive_page(old_exclusive_page);
            this->vector->data = data;
            this->vector->length = length;
        }
    }
};


