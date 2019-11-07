const libzfs = require('./build/Release/zfs');

var opts = {
    "name": "zones/some_uuid"
};

libzfs.zfsGet(opts, function(err, zfsInfo) {
	console.log('zfsInfo: ' + zfsInfo);
});
