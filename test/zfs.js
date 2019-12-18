var test = require('tape');
var zfs = require('../build/Release/zfs2.node');

var BASEFS = process.env.BASEFS;

var fsprops = {
    available: { type: 'number' },
    compressratio: { type: 'string' },
    createtxg: { type: 'number' },
    creation: { type: 'number' },
    filesystem_count: { type: 'number' },
    guid: { type: 'number' },
    inconsistent: { type: 'number' },
    logicalreferenced: { type: 'number' },
    logicalused: { type: 'number' },
    mounted: { type: 'string' },
    name: { type: 'string' },
    objsetid: { type: 'number' },
    origin: {type: 'string' },
    refcompressratio: { type: 'string' },
    referenced: { type: 'number' },
    snapshot_count: { type: 'number' },
    type: { type: 'string' },
    unique: { type: 'number' },
    used: { type: 'number' },
    usedbychildren: { type: 'number' },
    usedbydataset: { type: 'number' },
    usedbyrefreservation: { type: 'number' },
    usedbysnapshots:{ type: 'number' },
    useraccounting: { type: 'number' },
    written: { type: 'number' },
    aclinherit: { type: 'string' },
    aclmode: { type: 'string' },
    atime: { type: 'string' },
    aclmode: { type: 'string' },
    canmount: { type: 'string' },
    casesensitivity: { type: 'string' },
    checksum: { type: 'string' },
    compression: { type: 'string' },
    copies: { type: 'string' },
    dedup: { type: 'string' },
    devices: { type: 'string' },
    dnodesize: { type: 'string' },
    encryption: { type: 'string' },
    exec: { type: 'string' },
    filesystem_limit: { type: 'number' },
    keyformat: { type: 'string' },
    keylocation: { type: 'string' },
    logbias: { type: 'string' },
    mlslabel: { type: 'string' },
    mountpoint: { type: 'string' },
    nbmand: { type: 'string' },
    normalization: { type: 'string' },
    pbkdf2iters: { type: 'number' },
    pbkdf2salt: { type: 'number' },
    prevsnap: {type: 'string'},
    primarycache: { type: 'string' },
    quota: { type: 'number' },
    readonly: { type: 'string' },
    recordsize: { type: 'number' },
    redundant_metadata: { type: 'string' },
    refquota: { type: 'number' },
    refreservation: { type: 'number' },
    reservation: { type: 'number' },
    secondarycache: { type: 'string' },
    setuid: { type: 'string' },
    sharenfs: { type: 'string' },
    sharesmb: { type: 'string' },
    snapdir: { type: 'string' },
    snapshot_limit: { type: 'number' },
    special_small_blocks: { type: 'number' },
    sync: { type: 'string' },
    utf8only: { type: 'string' },
    version: { type: 'string' },
    vscan: {type: 'string' },
    xattr: { type: 'string' },
    zoned: { type: 'string' }
};


test('checkBaseFilesystem', function(t) {
	if (!BASEFS) {
		t.fail('BASEFS environment variable is not set');
		t.end();
		return;
	}

	t.pass();
	t.end();
});

test('zfsGet() throws exception when given invalid parameters', function(t) {
	t.plan(9);
	t.throws(zfs.zfsGet);
	t.throws(zfs.zfsGet.bind(null, 123));
	t.throws(zfs.zfsGet.bind(null, {}));
	t.throws(zfs.zfsGet.bind(null, {key: 'value'}));
	t.throws(zfs.zfsGet.bind(null, {name: 124.2}));
	t.throws(zfs.zfsGet.bind(null, {name: 'something'}));
	t.throws(zfs.zfsGet.bind(null, {name: 'something'}, 123));
	t.throws(zfs.zfsGet.bind(null, {name: 'something'}, 'string'));
	t.throws(zfs.zfsGet.bind(null, {name: 'something'}, {}));
});

test('zfsGet() filesystem does not exist', function(t) {
	zfs.zfsGet({name: '123' + BASEFS}, function(err, fsinfo) {
		if (err) {
			t.pass();
			t.end();
			return;
		}

		t.fail('Should not be able to open  a non-existing filesystem');
		t.end();
	});
});

test('zfsGet() succeedes', function(t) {
    t.plan(fsprops.length * 7 + 2);
    zfs.zfsGet({name: BASEFS}, function(err, fsinfo) {
        if (err) {
            t.fail('Failed to open:' + BASEFS);
            t.end();
            return;
        }
        t.ok(typeof fsinfo == 'object');
        Object.keys(fsinfo).forEach(function(key) {
            t.ok(typeof fsinfo[key] == 'object', 'fsinfo is an object')
            t.ok(fsinfo[key].hasOwnProperty('value', 'value property test'));
            t.ok(fsinfo[key].hasOwnProperty('source', 'source property test'));
            t.ok(fsinfo[key].hasOwnProperty('where'), 'where property test');

            t.ok(typeof fsinfo[key].source == 'string', fsinfo[key].source);
            t.ok(typeof fsinfo[key].where == 'string', fsinfo[key].where);

            if (fsprops[key]) {
                t.ok(typeof fsinfo[key].value == fsprops[key].type, key);
            } else {
                // Not in the prop table we have as of today. Maybe this is
                // a file system property added in the future!
                var value = JSON.stringify(fsinfo[key], null, 2);
                t.ok(true, key + ' = ' + value);
            }
        });

        // Confirm this is a filesystem dataset
        t.ok(fsinfo.type.value == 'filesystem');
        t.end();
    });
});
