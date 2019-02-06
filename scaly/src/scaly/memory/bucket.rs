use scaly::memory::bucket::Bucket::Heap;
use scaly::memory::bucket::Bucket::Stack;
use scaly::memory::heapbucket::HeapBucket;
use scaly::memory::page::Page;
use scaly::memory::page::PAGE_SIZE;
use scaly::memory::stackbucket::StackBucket;
use std::alloc::alloc;
use std::alloc::Layout;
use std::mem::size_of;
use std::ptr::null_mut;

pub const BUCKET_PAGES: usize = size_of::<usize>() * 8;

pub enum Bucket {
    Stack(StackBucket),
    Heap(HeapBucket),
}

impl Bucket {
    pub fn get(address: usize) -> *mut Bucket {
        ((address & !(PAGE_SIZE * BUCKET_PAGES - 1)) + size_of::<Page>()) as *mut Bucket
    }

    pub fn allocate_page(&mut self) -> *mut Page {
        match self {
            Stack(s) => s.allocate_page(),
            Heap(h) => h.allocate_page(),
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

    pub fn allocate_bucket() -> *mut Page {
        unsafe {
            let memory = alloc(Layout::from_size_align_unchecked(
                PAGE_SIZE * BUCKET_PAGES,
                PAGE_SIZE * BUCKET_PAGES,
            ));
            if memory == null_mut() {
                panic!("Unable to crete a new stack bucket: Out of memory.");
            }
            //println!("bucket memory:{:X}", memory as usize);
            let page = memory as *mut Page;
            (*page).reset();
            page
        }
    }
}
