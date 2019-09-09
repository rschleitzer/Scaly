use scaly::containers::Ref;
use scaly::Page;
use scalyc::parser::ProgramSyntax;

pub struct PlanBuilder {}

impl PlanBuilder {
    pub fn new(_rp: *mut Page) -> PlanBuilder {
        PlanBuilder {}
    }

    pub fn build(&mut self, _rp: *mut Page, _program_syntax: Ref<ProgramSyntax>) -> Ref<Module> {
        Ref::new(_rp, Module {})
    }
}

pub struct Module {}
