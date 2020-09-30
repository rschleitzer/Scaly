use std::collections::HashMap;

pub struct Model {
    pub modules: Vec<Item>,
}

impl Model {
    pub fn new() -> Model {
        Model {
            modules: Vec::new(),
        }
    }
}

pub struct Item {
    pub name: String,
    pub items: HashMap<String, Item>,
}

impl Item {
}
