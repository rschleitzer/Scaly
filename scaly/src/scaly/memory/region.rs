use scaly::memory::page::Page;
use scaly::memory::stackbucket::StackBucket;

pub struct Region<'a> {
    pub page: &'a mut Page,
}

impl<'a> Region<'a> {
    pub fn create_from_page(page: &Page) -> Region<'a> {
        let our_page_address = StackBucket::new_page(page);
        //println!("our_page_address:{:X}", our_page_address as usize);
        Region {
            page: unsafe { &mut *(our_page_address as *mut Page) },
        }
    }
    pub fn create(region: &Region<'a>) -> Region<'a> {
        Region::create_from_page(region.page)
    }

    pub fn new<T>(&mut self, object: T) -> &'a mut T {
        unsafe { &mut *(*self.page).allocate(object) }
    }

    pub fn allocate(&mut self, length: usize) -> *mut u8 {
        (*self.page).allocate_raw(length, 1)
    }
}

impl<'a> Drop for Region<'a> {
    fn drop(&mut self) {
        self.page.deallocate_extensions();
    }
}

// #[test]
// fn test_region() {
//     use scaly::memory::bucket::BUCKET_PAGES;
//     use scaly::memory::heap::Heap;
//     unsafe {
//         let mut heap = Heap::create();
//         let root_stack_bucket = StackBucket::create(&mut heap);
//         let mut r1 = Region::create_from_page(&*Page::get(root_stack_bucket as usize));
//         //println!("r1.page:{:X}", r1.page as *mut Page as usize);
//         let one = r1.new(1);
//         //println!("one:{:X}", one as *mut i32 as usize);
//         assert_eq!(*one, 1);
//         let two = r1.new(2);
//         //println!("two:{:X}", two as *mut i32 as usize);
//         assert_eq!(*two, 2);
//         {
//             let mut r2a = Region::create(&r1);
//             //println!("r2a.page:{:X}", r2a.page as *mut Page as usize);
//             let three = r2a.new(3);
//             //println!("three:{:X}", three as *mut i32 as usize);
//             assert_eq!(*three, 3);
//             let four = r2a.new(4);
//             //println!("four:{:X}", four as *mut i32 as usize);
//             assert_eq!(*four, 4);
//             assert_eq!(*three, 3);
//             let five = r2a.new(5);
//             //println!("five:{:X}", five as *mut i32 as usize);
//             assert_eq!(*five, 5);
//             assert_eq!(*four, 4);
//             assert_eq!(*three, 3);
//         }
//         {
//             let mut r2b = Region::create(&r1);
//             //println!("r2b.page:{:X}", r2b.page as *mut Page as usize);
//             let six = r2b.new(6);
//             //println!("six:{:X}", six as *mut i32 as usize);
//             assert_eq!(*six, 6);
//             let seven = r2b.new(7);
//             //println!("seven:{:X}", seven as *mut i32 as usize);
//             assert_eq!(*seven, 7);
//             assert_eq!(*six, 6);
//         }
//         // println!("one:{:X}", one as *mut i32 as usize);
//         // println!("one value: {}", *one);
//         assert_eq!(*one, 1);
//         assert_eq!(*two, 2);
//         // (*root_stack_bucket).deallocate();
//     }
//     unsafe {
//         use scaly::memory::heap::Heap;
//         let mut heap = Heap::create();
//         let root_stack_bucket = StackBucket::create(&mut heap);

//         let mut r = Region::create_from_page(&*Page::get(root_stack_bucket as usize));
//         for _ in 1..BUCKET_PAGES * 2 {
//             r = Region::create(&r);
//         }

//         //(*root_stack_bucket).deallocate();
//     }
// }
