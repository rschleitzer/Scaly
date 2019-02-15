use scaly::memory::bucket::BUCKET_PAGES;
use scaly::memory::pool::Pool;

pub struct Heap {
    map: usize,
    pools: [*mut Pool; BUCKET_PAGES],
}

impl Heap {}
