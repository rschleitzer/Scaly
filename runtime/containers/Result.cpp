namespace scaly::containers {

template<class OK, class ERROR>
struct Result {
    enum {
        Ok,
        Error,
    } tag;
    union {
        OK ok;
        ERROR error;
    };
};

}