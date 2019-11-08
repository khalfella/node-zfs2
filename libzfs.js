const libzfs = require('./build/Release/zfs');

var opts = {
    "name": "zones/152a2d85-d39e-ec93-aa51-8108fef3320b/data"
};

libzfs.zfsGet(opts, function(err, zfsInfo) {
	if (err) {
		console.error('error, something went wrong');
		return;
	}
	console.log(JSON.stringify(zfsInfo));
});
