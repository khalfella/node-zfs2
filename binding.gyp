{
    "targets": [
        {
            "target_name": "zfs2",
            "sources": [
                "src/zfs.cpp",
                "src/zfs_get_worker.cpp",
                "src/zpool_status_worker.cpp"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ],
            "link_settings": {
                "libraries": [ "-lzfs" ]
            }
        }
    ]
}
