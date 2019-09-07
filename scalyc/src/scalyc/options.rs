use scaly::containers::Ref;
use scaly::memory::Region;
use scaly::Equal;
use scaly::Page;
use scaly::{Array, String, Vector};

pub struct Options {
    pub files: Ref<Vector<String>>,
    pub output_name: Option<String>,
    pub directory: Option<String>,
    pub repl: bool,
}

impl Options {
    pub fn parse_arguments(
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
        arguments: Ref<Vector<String>>,
    ) -> Ref<Options> {
        let _r = Region::create(_pr);
        let mut output_name: Option<String> = None;
        let mut directory: Option<String> = None;
        let mut files: Ref<Array<String>> = Ref::new(_r.page, Array::new());
        let mut repl = false;

        let mut args = arguments.iter();
        loop {
            let mut arg = args.next();
            if let None = arg {
                break;
            }

            {
                let _r_1 = Region::create(&_r);
                if (arg.unwrap()).equals(&String::from_string_slice(_r_1.page, "-o")) {
                    arg = args.next();
                    output_name = Some(*arg.unwrap());
                    continue;
                }
            }

            {
                let _r_1 = Region::create(&_r);
                if (arg.unwrap()).equals(&String::from_string_slice(_r_1.page, "-d")) {
                    arg = args.next();
                    directory = Some(*arg.unwrap());
                    continue;
                }
            }

            {
                let _r_1 = Region::create(&_r);
                if (arg.unwrap()).equals(&String::from_string_slice(_r_1.page, "-r")) {
                    repl = true;
                    continue;
                }
            }

            files.add(*arg.unwrap());
        }

        Ref::new(
            _rp,
            Options {
                files: Ref::new(_rp, Vector::from_array(_rp, files)),
                output_name: output_name,
                directory: directory,
                repl: repl,
            },
        )
    }
}
