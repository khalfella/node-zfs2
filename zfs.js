const zfs = require('./build/Release/zfs2');

var fsName = "zones/66921791-ce69-ee88-bceb-93f91c0bd2f7/data";
var opts = {
    "name": fsName
};

zfs.zfsGet(opts, function(err, fsInfo) {
	if (err) {
		console.error('zfsGet(): error, something went wrong');
		return;
	}

	console.log(opts.name + ' has ' + JSON.stringify(fsInfo.available) + ' bytes available');
});

zfs.zpoolStatus({"name": "zones"}, function (err, zpoolStatus) {
	if (err) {
		console.error('zpoolStatus(): error, something went wrong');
		return;
	}
	console.log('zones pool status is: ' + zpoolStatus);
});
