from methods import print_warning

def can_build(env, platform):
    if env["arch"].startswith("rv"):
        return False

    if env.editor_build:
        env.module_add_dependencies("mono", ["regex"])

    return True


def configure(env):
    # Check if the platform has marked mono as supported.
    supported = env.get("supported", [])
    if "mono" not in supported:
        import sys

        print("The 'mono' module does not currently support building for this platform. Aborting.")
        sys.exit(255)

    # adds too much work for libgodot, so I just rebuild it fully with extra_suffix for now
    # return it to what it was later
    # env.add_module_version_string("mono")
    env.extra_suffix = env.extra_suffix + ".mono"
        
    if env["library_type"] != "executable" and not env["disable_crash_handler"]:
        print_warning(
            'CoreCLR installs crash handler itself, please use "disable_crash_handler=yes" if you '
            'plan to use Godot as a C# library.'
        )

    # Doesn't work right now.
    if env["platform"] == "web" and env["library_type"] == "shared_library":
        env["EXPORTED_FUNCTIONS_SHARED"] += ["_set_load_from_executable_fn"]


def get_doc_classes():
    return [
        "CSharpScript",
        "GodotSharp",
    ]


def get_doc_path():
    return "doc_classes"


def is_enabled():
    # The module is disabled by default. Use module_mono_enabled=yes to enable it.
    return False
