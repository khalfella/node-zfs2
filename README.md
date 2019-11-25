# node-zfs2

_This is a WIP, and it is not production-ready yet._

node-zfs2 is a thin wrapper around libzfs.

## Usage

```
const zfs = require('./build/Release/zfs2');

var fsName = "zones/66921791-ce69-ee88-bceb-93f91c0bd2f7/data";
var opts = {
    "name": fsName
};

zfs.zfsGet(opts, function(err, fsInfo) {
        if (err) {
                console.error('error, something went wrong');
                return;
        }

        console.log(fsName + ' has ' + fsInfo.available.value + ' bytes available');
});

```

```
# node zfs2.js
zones/66921791-ce69-ee88-bceb-93f91c0bd2f7/data has 107245799424 bytes available
#
```
