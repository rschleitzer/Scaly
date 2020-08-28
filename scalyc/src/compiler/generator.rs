use crate::compiler::planner::*;
use llvm_sys::core::*;
use llvm_sys::*;
use std::ffi::CString;

pub struct Generator {
    context: *mut llvm_sys::LLVMContext,
}

impl Generator {
    pub fn new() -> Generator {
        Generator {
            context: unsafe { LLVMContextCreate() },
        }
    }

    pub fn generate(&self, plan: Plan) {
        unsafe {
            let c_module_name = CString::new(plan.name).expect("invalid module name.");
            let module = LLVMModuleCreateWithNameInContext(c_module_name.as_ptr(), self.context);
            for function in &plan.functions {
                self.add_function(module, function)
            }
        }
    }

    fn add_function(&self, module: *mut LLVMModule, function: &Function) {
        unsafe {
            let i64t = LLVMInt64TypeInContext(self.context);
            let mut argts: [*mut LLVMType; 0] = [];
            let function_type = LLVMFunctionType(i64t, argts.as_mut_ptr(), argts.len() as u32, 0);
            let name = CString::new(function.name.clone()).expect("invalid function name.");
            let llvm_function = LLVMAddFunction(module, name.as_ptr(), function_type);
            for block in &function.blocks {
                self.add_block(llvm_function, block)
            }
        }
    }

    fn add_block(&self, function: *mut LLVMValue, block: &Block) {
        unsafe {
            let name = CString::new(block.name.clone()).expect("invalid block name.");
            let llvm_block = LLVMAppendBasicBlockInContext(self.context, function, name.as_ptr());
            let builder = LLVMCreateBuilderInContext(self.context);
            LLVMPositionBuilderAtEnd(builder, llvm_block);
            for instruction in &block.instructions {
                self.add_instruction(llvm_block, instruction)
            }
        }
    }

    fn add_instruction(&self, block: *mut LLVMBasicBlock, instruction: &Instruction) {
        match instruction {
            Instruction::Ret(value) => self.add_ret(block, value),
        }
    }

    fn add_ret(&self, _block: *mut LLVMBasicBlock, _value: &Value) {}
}

impl Drop for Generator {
    fn drop(&mut self) {
        unsafe { LLVMContextDispose(self.context) };
    }
}
