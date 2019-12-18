var test = require('tape');
var zfs = require('../build/Release/zfs2.node');

var BASEFS = process.env.BASEFS;

test('validate BASEFS', function(t) {
    if ((BASEFS && BASEFS.match(/.+\/.+/))) {
        t.pass();
        t.end();
        return;
    }

    t.fail('Invalid BASEFS: ' + BASEFS)
    t.end();
});


test('zpoolStatus() throws an excepeion', function(t) {
    t.plan(9);
    t.throws(zfs.zpoolStatus);
    t.throws(zfs.zpoolStatus.bind(null, 123));
    t.throws(zfs.zpoolStatus.bind(null, {}));
    t.throws(zfs.zpoolStatus.bind(null, {key: 'value'}));
    t.throws(zfs.zpoolStatus.bind(null, {name: 124.2}));
    t.throws(zfs.zpoolStatus.bind(null, {name: 'something'}));
    t.throws(zfs.zpoolStatus.bind(null, {name: 'something'}, 123));
    t.throws(zfs.zpoolStatus.bind(null, {name: 'something'}, 'string'));
    t.throws(zfs.zpoolStatus.bind(null, {name: 'something'}, {}));
    t.end();
});

test('zpoolStatus() error on non-existent pool', function(t) {
    t.plan(1);
    var invalidPoolName = 'ihopethereisnopoolwiththisname';
    zfs.zpoolStatus({name: invalidPoolName},
        function(err, poolStatus) {
        t.ok(err)
        t.end();
    });
});

test('zpoolStatus() on an online pool', function(t) {
    t.plan(4);
    var zpoolName = BASEFS.split('/')[0];
    t.ok(zpoolName);
    zfs.zpoolStatus({name: zpoolName}, function (err, zpoolStatus) {
        t.ok(!err);
        t.ok(zpoolStatus == 'ONLINE');
        t.end();
    })
});
