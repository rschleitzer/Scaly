extern crate llvm_sys as llvm;
use llvm::core::*;
use llvm::execution_engine::*;
use llvm::target::*;
use std::mem;

mod parser;
use parser::Parser;
use parser::StatementSyntax;

pub struct Compiler {}

impl Compiler {
    pub fn new() -> Compiler {
        Compiler {}
    }
    pub fn evaluate(&mut self, deck: &str) -> String {
        let mut parser = Parser::new(deck);
        let statement_result = parser.parse_statement();
        match statement_result {
            Ok(statement_option) => match statement_option {
                None => {
                    if parser.is_at_end() {
                        return String::new();
                    }

                    return String::from("This is no statement.\n");
                }
                Some(statement) => {
                    if !parser.is_at_end() {
                        return String::from(format!(
                            "Unexpected character at {}, {} after the statement.\n",
                            parser.get_current_line(),
                            parser.get_current_column()
                        ));
                    }
                    self.compile_statement(&statement)
                }
            },
            Err(error) => {
                return String::from(format!(
                    "Parser error at {}, {}\n",
                    error.line, error.column
                ));
            }
        }
    }

    fn compile_statement(&mut self, _statement: &StatementSyntax) -> String {
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
            let bb =
                LLVMAppendBasicBlockInContext(context, function, b"entry\0".as_ptr() as *const _);

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
