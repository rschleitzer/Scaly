use scaly::memory::bucket::Bucket::Heap;
use scaly::memory::bucket::Bucket::Stack;
use scaly::memory::heapbucket::HeapBucket;
use scaly::memory::page::Page;
use scaly::memory::page::PAGE_SIZE;
use scaly::memory::pool::Pool;
use scaly::memory::stackbucket::StackBucket;
use std::mem::size_of;

pub const BUCKET_PAGES: usize = size_of::<usize>() * 8;

pub enum Bucket {
    Stack(StackBucket),
    Heap(HeapBucket),
}

impl Bucket {
    pub fn get(address: usize) -> *mut Bucket {
        // println!("address: {:X}", address);
        let address_masked = address & !(PAGE_SIZE * BUCKET_PAGES - 1);
        // println!("address_masked: {:X}", address_masked);
        (address_masked + size_of::<Page>()) as *mut Bucket
    }

    pub fn allocate_page(&mut self) -> *mut Page {
        match self {
            Stack(s) => s.allocate_page(),
            Heap(h) => h.allocate_page(),
        }
    }

    pub fn set_pool(&mut self, pool: *mut Pool) {
        match self {
            Stack(s) => s.pool = pool,
            Heap(h) => h.pool = pool,
        }
    }

    pub fn deallocate_page(&mut self, page: &Page) {
        match self {
            Heap(h) => h.deallocate_page(page),
            Stack(_) => panic!(
                "Tried to deallocate the page {:X} from a StackBucket.",
                page as *const Page as usize
            ),
        }
    }

    pub fn find_least_position(n: usize) -> usize {
        match BUCKET_PAGES {
            64 => Bucket::find_least_position_64(n),
            32 => Bucket::find_least_position_32(n),
            _ => panic!(
                "Scaly currently does not support {}bit architecture.",
                BUCKET_PAGES
            ),
        }
    }

    // from http://stackoverflow.com/questions/11376288/fast-computing-of-log2-for-64-bit-integers
    fn find_least_position_64(n: usize) -> usize {
        static NUMBERS: &'static [usize] = &[
            0, 58, 1, 59, 47, 53, 2, 60, 39, 48, 27, 54, 33, 42, 3, 61, 51, 37, 40, 49, 18, 28, 20,
            55, 30, 34, 11, 43, 14, 22, 4, 62, 57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10,
            13, 21, 56, 45, 25, 31, 35, 16, 9, 12, 44, 24, 15, 8, 23, 7, 6, 5, 63,
        ];

        let n = n | n >> 1;
        let n = n | n >> 2;
        let n = n | n >> 4;
        let n = n | n >> 8;
        let n = n | n >> 16;
        let n = n | n >> 32;

        let product = n as u128 * 0x03f6eaf2cd271461;

        let log = NUMBERS[product as usize >> 58];
        // println!("Log: {}", log);
        BUCKET_PAGES - log
    }

    // from http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
    fn find_least_position_32(n: usize) -> usize {
        static NUMBERS: &'static [usize] = &[
            0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30, 8, 12, 20, 28, 15, 17, 24,
            7, 19, 27, 23, 6, 26, 5, 4, 31,
        ];

        let n = n | n >> 1;
        let n = n | n >> 2;
        let n = n | n >> 4;
        let n = n | n >> 8;
        let n = n | n >> 16;

        let log = NUMBERS[(n * 0x07C4ACDD) >> 27];
        BUCKET_PAGES - log
    }
}
