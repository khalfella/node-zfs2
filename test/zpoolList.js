var test = require('tape');
var zfs = require('../build/Release/zfs2.node');

var BASEFS = process.env.BASEFS;

var zpoolprops = {
    allocated: { type: 'number' },
    capacity: { type: 'number' },
    checkpoint: { type: 'string' },
    dedupratio: { type: 'string' },
    expandsize: { type: 'string' },
    fragmentation: { type: 'string' },
    free: { type: 'number' },
    freeing: { type: 'number' },
    guid: { type: 'number' },
    health: { type: 'string' },
    leaked: { type: 'number' },
    maxblocksize: { type: 'number' },
    maxdnodesize: { type: 'number' },
    name: { type: 'string' },
    size: { type: 'number' },
    altroot: { type: 'string' },
    ashift: { type: 'number' },
    autoexpand: { type: 'string' },
    autoreplace: { type: 'string' },
    autotrim: { type: 'string' },
    bootfs: { type: 'string' },
    bootsize: { type: 'string' },
    cachefile: { type: 'string' },
    comment: { type: 'string' },
    dedupditto: { type: 'string' },
    delegation: { type: 'string' },
    failmode: { type: 'string' },
    listsnapshots: { type: 'string' },
    multihost: { type: 'string' },
    readonly: { type: 'string' },
    tname: { type: 'string' },
    version: { type: 'string' }
};

test('validate BASEFS', function(t) {
    if ((BASEFS && BASEFS.match(/.+\/.+/))) {
        t.pass();
        t.end();
        return;
    }


    t.fail('Invalid BASEFS: ' + BASEFS)
    t.end();
});

test('zpoolList() throws an exception', function(t) {
	t.plan(9);
	t.throws(zfs.zpoolStatus);
	t.throws(zfs.zpoolList.bind(null, 123));
	t.throws(zfs.zpoolList.bind(null, {}));
	t.throws(zfs.zpoolList.bind(null, {key: 'value'}));
	t.throws(zfs.zpoolList.bind(null, {name: 124.2}));
	t.throws(zfs.zpoolList.bind(null, {name: 'something'}));
	t.throws(zfs.zpoolList.bind(null, {name: 'something'}, 123));
	t.throws(zfs.zpoolList.bind(null, {name: 'something'}, 'string'));
	t.throws(zfs.zpoolList.bind(null, {name: 'something'}, {}));
	t.end();
});

test('zpoolList()', function(t) {
    zfs.zpoolList({}, function(err, zpoolList) {
        t.ok(!err);
        t.ok(Array.isArray(zpoolList));


        zpoolList.forEach(function(zpool) {
            t.ok(typeof zpool == 'object');
            Object.keys(zpool).forEach(function(key) {
                t.ok(typeof key == 'string');
                t.ok(zpool[key].hasOwnProperty('value'));
                t.ok(zpool[key].hasOwnProperty('source'));
                t.ok(typeof zpool[key].source == 'string');

                if(zpoolprops[key]) {
                    t.ok(typeof zpool[key].value == zpoolprops[key].type, key);
                } else {
                    // This is a property we don't know of.
                    var value = JSON.stringify(zpool[key], null, 2);
                    t.ok(true, key + ' = ' + value);
                }
            });
        });


        t.end();
    });
});
