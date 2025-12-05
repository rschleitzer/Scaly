namespace scaly {
namespace containers {

template<class OK, class ERROR>
struct Result {
    enum {
        Ok,
        Error,
    } _tag;
    union {
        OK _Ok;
        ERROR _Error;
    };
};

}
}