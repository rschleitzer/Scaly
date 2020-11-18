use std::fs;

extern crate llvm_sys as llvm;
use llvm::core::*;
use llvm::execution_engine::*;
use llvm::target::*;
use model::Model;
use std::mem;

mod parser;
use parser::OperationSyntax;
use parser::FileSyntax;
use parser::Parser;
use parser::ProgramSyntax;

mod planner;

mod model;

mod generator;

pub struct Compiler {
    standard_library: Option<String>,
}

impl Compiler {
    pub fn new() -> Compiler {
        Compiler {
            standard_library: None,
        }
    }

    pub fn load_standard_library(&mut self) {
        let contents_result = fs::read_to_string("scaly/scaly.scaly");
        match contents_result {
            Ok(contents) => {
                self.standard_library = Some(contents);
            }
            Err(_) => (),
        }
    }

    pub fn compile(&mut self, file_name: String) -> String
    {
        let file_contents_result = fs::read_to_string(file_name.as_str());
        match file_contents_result {
            Ok(file_contents) => {
                let mut parser = Parser::new(file_contents.as_str());
                let file_result = parser.parse_file(file_name);
                match file_result {
                    Ok(file_option) => match file_option {
                        None => {        
                            return String::from("Unable to parse file.\n");
                        }
                        Some(file) => {
                            if !parser.is_at_end() {
                                return String::from(format!(
                                    "Unexpected characters between {}, {} and {}, {} after the file.\n",
                                    parser.get_previous_line(),
                                    parser.get_previous_column(),
                                    parser.get_current_line(),
                                    parser.get_current_column()
                                ));
                            }
                            match self.compile_file(file) {
                                Ok(result) => result,
                                Err(error) => error,
                            }
                        }
                    },
                    Err(error) => {
                        if parser.is_at_end() {
                            String::from(format!(
                                "Parser error between {}, {} and {}, {}.\n",
                                parser.get_previous_line(),
                                parser.get_previous_column(),
                                parser.get_current_line(),
                                parser.get_current_column()
                            ))
                        }
                        else {
                            String::from(format!(
                            "Parser error at {}, {}\n",
                            error.line, error.column))
                        }
                    }
                }
            },
            Err(_) => String::from("Unable to read file.")
        }
    }

    pub fn compile_file(&mut self, file: FileSyntax) -> Result<String, String> {
        let mut program = ProgramSyntax { files: Vec::new() };
        self.add_standard_library(&mut program);
        program.files.push(file);
        let mut model = Model::new();
        model.build(program);
        Ok(String::from("Processed."))
    }

    pub fn evaluate_file(&mut self, file: &str) -> Result<String, String> {
        let mut parser = Parser::new(file);
        let file_result = parser.parse_file(String::from("<console>"));
        match file_result {
            Ok(file_option) => match file_option {
                None => {
                    if parser.is_at_end() {
                        return Ok(String::new());
                    }

                    return Err(String::from("This is no statement or declaration.\n"));
                }
                Some(file) => {
                    if !parser.is_at_end() {
                        return Err(String::from(format!(
                            "Unexpected characters between {}, {} and {}, {} after the statement.\n",
                            parser.get_previous_line(),
                            parser.get_previous_column(),
                            parser.get_current_line(),
                            parser.get_current_column()
                        )));
                    }
                    {
                        let mut program = ProgramSyntax { files: Vec::new() };
                        self.add_standard_library(&mut program);
                        program.files.push(file);
                        let mut model = Model::new();
                        model.build(program);
                        Ok(String::from("Processed."))
                    }
                }
            },
            Err(error) => {
                if parser.is_at_end() {
                    return Err(String::new());
                }
                return Err(String::from(format!(
                    "Parser error at {}, {}\n",
                    error.line, error.column
                )));
            }
        }
    }

    fn add_standard_library(&mut self, program: &mut ProgramSyntax) -> Option<String>
    {
        if let Some(standard_library) = &self.standard_library {
            let mut parser = Parser::new(standard_library.as_ref());
            let result = parser.parse_file(String::from("scaly/scaly.scaly"));
            match result {
                Ok(file_syntax_option) => 
                    match file_syntax_option {
                        Some(file_syntax) => program.files.push(file_syntax),
                        None => ()
                    }
                Err(_) => return Some(String::from(format!(
                    "Failed to parsed standard library. Unexpected characters between {}, {} and {}, {} after the statement.\n",
                    parser.get_previous_line(),
                    parser.get_previous_column(),
                    parser.get_current_line(),
                    parser.get_current_column()
                ))),
            }
        }
        None
    }

    fn _compute(&mut self, _operation: &OperationSyntax) -> String {
        self._jit_and_execute()
    }

    fn _jit_and_execute(&mut self) -> String {
        unsafe {
            let context = LLVMContextCreate();
            let module = LLVMModuleCreateWithNameInContext(b"sum\0".as_ptr() as *const _, context);
            let builder = LLVMCreateBuilderInContext(context);
            let i64t = LLVMInt64TypeInContext(context);
            let mut argts = [i64t, i64t, i64t];
            let function_type = LLVMFunctionType(i64t, argts.as_mut_ptr(), argts.len() as u32, 0);

            // add it to our module
            let function = LLVMAddFunction(module, b"sum\0".as_ptr() as *const _, function_type);

            // Create a basic block in the function and set our builder to generate
            // code in it.
            let bb = LLVMAppendBasicBlockInContext(context, function, b"\0".as_ptr() as *const _);

            LLVMPositionBuilderAtEnd(builder, bb);

            // get the function's arguments
            let x = LLVMGetParam(function, 0);
            let y = LLVMGetParam(function, 1);
            let z = LLVMGetParam(function, 2);

            let sum = LLVMBuildAdd(builder, x, y, b"sum.1\0".as_ptr() as *const _);
            let sum = LLVMBuildAdd(builder, sum, z, b"sum.2\0".as_ptr() as *const _);

            // Emit a `ret void` into the function
            LLVMBuildRet(builder, sum);

            // done building
            LLVMDisposeBuilder(builder);

            // Dump the module as IR to stdout.
            LLVMDumpModule(module);
            let mut out = mem::zeroed();

            // robust code should check that these calls complete successfully
            // each of these calls is necessary to setup an execution engine which compiles to native
            // code
            LLVMLinkInMCJIT();
            LLVM_InitializeNativeTarget();
            LLVM_InitializeNativeAsmPrinter();

            // build an execution engine
            let mut ee = mem::MaybeUninit::uninit();
            // takes ownership of the module
            LLVMCreateExecutionEngineForModule(ee.as_mut_ptr(), module, &mut out);
            let ee = ee.assume_init();
            let addr = LLVMGetFunctionAddress(ee, b"sum\0".as_ptr() as *const _);

            let f: extern "C" fn(u64, u64, u64) -> u64 = mem::transmute(addr);

            let x: u64 = 1;
            let y: u64 = 1;
            let z: u64 = 1;
            let res = f(x, y, z);

            println!("{} + {} + {} = {}", x, y, z, res);

            // Clean up the rest.
            LLVMDisposeExecutionEngine(ee);
            LLVMContextDispose(context);

            String::from("Parsed successfully.\n")
        }
    }
}
