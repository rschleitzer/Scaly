use llvm_sys::core::LLVMContextCreate;
use llvm_sys::core::LLVMContextDispose;
use llvm_sys::core::LLVMModuleCreateWithNameInContext;
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
    pub fn generate(&self, module_name: String) {
        unsafe {
            let c_module_name =
                CString::new(module_name).expect("module_name could not be converted.");
            let module = LLVMModuleCreateWithNameInContext(c_module_name.as_ptr(), self.context);
        }
    }
}

impl Drop for Generator {
    fn drop(&mut self) {
        unsafe { LLVMContextDispose(self.context) };
    }
}
