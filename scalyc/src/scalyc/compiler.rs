use scaly::containers::{Array, Ref};
use scaly::io::{Console, Disposer, File, Stream};
use scaly::memory::Region;
use scaly::Page;
use scaly::String;
use scaly::Vector;
use scalyc::errors::ParserError;
use scalyc::modulebuilder::ModuleBuilder;
use scalyc::options::Options;
use scalyc::parser::{FileSyntax, Parser, ProgramSyntax};

pub struct Compiler {
    arguments: Ref<Vector<String>>,
}

impl Compiler {
    pub fn new(_p: *mut Page, arguments: Ref<Vector<String>>) -> Compiler {
        Compiler {
            arguments: arguments,
        }
    }

    pub fn compile(&self, _pr: &Region, _rp: *mut Page, _ep: *mut Page) {
        let _r = Region::create(_pr);
        let options = Options::parse_arguments(&_r, _r.page, _ep, self.arguments);
        if let Some(output_name) = options.output_name {
            match self.parse_files(&_r, _r.page, _r.page, options.files) {
                Ok(program_syntax) => {
                    self.build_module(&_r, _r.page, _r.page, program_syntax, output_name);
                }
                Err(_) => (),
            }
        }

        if options.repl {
            self.run_repl(&_r);
        }
    }

    fn build_module(
        &self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
        program_syntax: Ref<ProgramSyntax>,
        program_name: String,
    ) {
        let _r = Region::create(_pr);
        let mut module_builder = Ref::new(_r.page, ModuleBuilder::new(_r.page, program_name));
        module_builder.build(&_r, _rp, program_syntax)
    }

    fn parse_files(
        &self,
        _pr: &Region,
        _rp: *mut Page,
        _ep: *mut Page,
        file_names: Ref<Vector<String>>,
    ) -> Result<Ref<ProgramSyntax>, Ref<ParserError>> {
        let files = {
            let _r = Region::create(_pr);
            let mut array: Ref<Array<Ref<FileSyntax>>> = Ref::new(_r.page, Array::new());
            for file in file_names.iter() {
                let _r_1 = Region::create(&_r);
                let file_result = File::open_read(&_r_1, _r_1.page, *file);
                match file_result {
                    Ok(file_stream) => {
                        let _file_stream_disposer = Disposer {
                            stream: file_stream as *mut Stream,
                        };
                        let mut parser =
                            Ref::new(_r_1.page, Parser::new(&_r_1, _r.page, *file, file_stream));
                        match parser.parse_file(&_r_1, _rp, _ep) {
                            Ok(file_syntax_option) => {
                                if let Some(file_syntax) = file_syntax_option {
                                    array.add(file_syntax)
                                }
                            }
                            Err(_) => continue,
                        }
                    }
                    Err(_) => continue,
                }
            }
            Ref::new(_r.page, Vector::from_array(_r.page, array))
        };

        Ok(Ref::new(_rp, ProgramSyntax { files }))
    }

    fn run_repl(&self, _pr: &Region) {
        let _r = Region::create(_pr);
        loop {
            let _r_1 = Region::create(&_r);
            Console::write(&_r_1, String::from_string_slice(_r_1.page, "Scaly>"));
            let mut parser = Ref::new(
                _r_1.page,
                Parser::new(
                    &_r_1,
                    _r_1.page,
                    String::from_string_slice(_r_1.page, "<stdin>"),
                    Console::open_standard_output(_r_1.page),
                ),
            );
            match parser.parse_statement(&_r_1, _r_1.page, _r_1.page) {
                Ok(optional_statement) => {
                    if let Some(_statement) = optional_statement {

                    } else {
                        break;
                    }
                }
                Err(_) => break,
            }
        }
    }
}
