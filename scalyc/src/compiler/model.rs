use std::collections::HashMap;

pub struct Model {
    pub modules: HashMap<Box<str>, Module>,
}

impl Model {
    pub fn new() -> Model {
        Model {
            modules: HashMap::new(),
        }
    }

    pub fn get_main(&mut self) -> &mut Module {
        self.modules
            .entry(Box::from("main"))
            .or_insert(Module::new())
    }
}

pub struct Module {}

impl Module {
    pub fn new() -> Module {
        Module {}
    }
}
