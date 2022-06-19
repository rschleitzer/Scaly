namespace scaly {
namespace containers {

using namespace scaly::memory;
using namespace scaly::containers;

template<class T>
struct HashSet : Object {
    Vector<Vector<T>>* slots;

    HashSet<T>(Region& _pr, Page* _rp, HashSetBuilder<T>& hash_set_builder) {
        auto _r = Region::create(_pr);
        if (hash_set_builder.length == 0)
            return;

        this->slots = new(alignof(Vector<Vector<T>>), _rp) Vector<Vector<T>>(_rp, hash_set_builder.slots->length);
        auto length = hash_set_builder.slots->length;
        for (size_t i = 0; i < length; i++) {
            auto _r_1 = Region::create(_r);
            Array<T>& array = *new(alignof(Array<T>), _r_1.page) Array<T>();
            auto list_iterator = ListIterator<Slot<T>>(hash_set_builder.slots->get(i)->head);
            while (auto item = list_iterator.next())
                array.add(item->value);
            if (array.length > 0)
                this->slots->set(i, Vector<T>(_rp, array));
        }
    }

    bool contains(const T& value) {
        if (this->slots == nullptr)
            return false;

        auto hash = value.hash();
        auto slot_number = hash % this->slots->length;
        auto slot = this->slots->get(slot_number);
        auto length = slot->length;
        for (size_t i = 0; i < length; i++) {
            if (value.equals(*slot->get(i))) {
                return true;
            }
        }

        return false;
    }

};

}
}