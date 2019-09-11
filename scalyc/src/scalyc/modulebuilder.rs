use scaly::containers::Ref;
use scaly::Page;
//use scaly::String;
use scalyc::parser::*;

pub struct ModuleBuilder {
    module: Option<Ref<Module>>,
}

impl ModuleBuilder {
    pub fn new(_rp: *mut Page) -> ModuleBuilder {
        ModuleBuilder { module: None }
    }

    pub fn build(&mut self, _rp: *mut Page, mut program_syntax: Ref<ProgramSyntax>) {
        program_syntax.accept(self);
    }
}

impl SyntaxVisitor for ModuleBuilder {
    fn open_program(&mut self, _program_syntax: Ref<ProgramSyntax>) -> bool {
        let _own_page = Page::own(Ref::from(self as *mut ModuleBuilder));
        self.module = Some(Ref::new(_own_page, Module {}));
        true
    }

    fn close_program(&mut self, _program_syntax: Ref<ProgramSyntax>) {}

    fn open_file(&mut self, _file_syntax: Ref<FileSyntax>) -> bool {
        if let Some(_module) = self.module {
            // if _file_syntax.file_name.equals(&String::from_string_slice(_r_1.page, "-o")) {

            // }
        }
        true
    }

    fn close_file(&mut self, _file_syntax: Ref<FileSyntax>) {}

    fn visit_intrinsic(&mut self, _intrinsic_syntax: Ref<IntrinsicSyntax>) {}

    fn open_calculation(&mut self, _calculation_syntax: Ref<CalculationSyntax>) -> bool {
        true
    }

    fn close_calculation(&mut self, _calculation_syntax: Ref<CalculationSyntax>) {}

    fn open_operation(&mut self, _operation_syntax: Ref<OperationSyntax>) -> bool {
        true
    }

    fn close_operation(&mut self, _operation_syntax: Ref<OperationSyntax>) {}

    fn open_operand(&mut self, _operand_syntax: Ref<OperandSyntax>) -> bool {
        true
    }

    fn close_operand(&mut self, _operand_syntax: Ref<OperandSyntax>) {}

    fn visit_constant(&mut self, _constant_syntax: Ref<ConstantSyntax>) {}
}

#[derive(Copy, Clone)]
pub struct Module {}
