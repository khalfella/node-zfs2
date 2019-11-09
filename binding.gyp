{
    "targets": [
        {
            "target_name": "zfs2",
            "sources": [ "zfs2.cpp" ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ],
            "link_settings": {
                "libraries": [ "-lzfs" ]
            }
        }
    ]
}
