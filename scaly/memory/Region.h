namespace scaly {
namespace memory {

struct Region {
    Region();
    ~Region();
    Page* get_page();

private:
    Page* page;
};

}

}