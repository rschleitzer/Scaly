use std::fs;

extern crate llvm_sys as llvm;
use llvm::core::*;
use llvm::execution_engine::*;
use llvm::target::*;
use modeler::Model;
use std::mem;

mod parser;
use parser::CalculationSyntax;
use parser::FileSyntax;
use parser::Parser;
use parser::ProgramSyntax;

mod planner;

mod modeler;

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
        let contents_result = fs::read_to_string("stdlib.scaly");
        match contents_result {
            Ok(contents) => {
                self.standard_library = Some(contents);
            }
            Err(_) => (),
        }
    }

    pub fn evaluate(&mut self, file: &str) -> Result<String, String> {
        let mut parser = Parser::new(file);
        let file_result = parser.parse_file();
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
                    self.process_file(file)
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

    fn process_file(&mut self, file: FileSyntax) -> Result<String, String> {
        let mut program = ProgramSyntax { files: Vec::new() };

        match &self.standard_library {
            Some(standard_library) => {
                let mut parser = Parser::new(standard_library.as_ref());
                let result = parser.parse_file();
                match result {
                    Ok(file_syntax_option) => 
                        match file_syntax_option {
                            Some(file_syntax) => program.files.push(file_syntax),
                            None => ()
                        }
                    Err(_) => return Err(String::from(format!(
                        "Failed to parsed standard library. Unexpected characters between {}, {} and {}, {} after the statement.\n",
                        parser.get_previous_line(),
                        parser.get_previous_column(),
                        parser.get_current_line(),
                        parser.get_current_column()
                    ))),
                }
            },
            None => ()
        }

        program.files.push(file);

        let _model = Model::new();

        Ok(String::from("Processed."))
    }

    fn _compute(&mut self, _calculation: &CalculationSyntax) -> String {
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
