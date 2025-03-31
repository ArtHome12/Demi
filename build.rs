// build.rs
extern crate embed_resource;

fn main() {
    // Run resource compilation only when building under Windows
    if std::env::var("TARGET").unwrap().contains("windows") {
         println!("cargo:rerun-if-changed=res/app.rc"); // Rebuild if .rc has changed
         println!("cargo:rerun-if-changed=res/app.ico"); // Rebuild if .ico has changed
         println!("Result={}", embed_resource::compile("res/app.rc", embed_resource::NONE));
    }
}