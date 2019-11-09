const libzfs = require('./build/Release/zfs2');

var opts = {
    "name": "zones/66921791-ce69-ee88-bceb-93f91c0bd2f7/data"
};

libzfs.zfsGet(opts, function(err, zfsInfo) {
	console.log('callback being called....');
	if (err) {
		console.error('error, something went wrong');
		return;
	}

	console.log(JSON.stringify(zfsInfo, null, 2));
});
