{
    "targets": [
        {
            "target_name": "libzfs",
            "sources": [ "libzfs.cpp" ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ],
            "link_settings": {
                "libraries": [ "-lzfs" ]
            }
        }
    ]
}
