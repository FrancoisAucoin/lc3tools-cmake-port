Module['preInit'] = Module['preInit'] || [];
Module['preInit'].push(function () {
    // if (!Module.myStdin) {
    //     throw new Error(`Expecting module config object to contain 'myStdin' callback`);
    // }
    if (!Module.myStdout) {
        throw new Error(`Expecting module config object to contain 'myStdout' callback`);
    }
    if (!Module.myStderr) {
        throw new Error(`Expecting module config object to contain 'myStderr' callback`);
    }
    FS.init(null, Module.myStdout, Module.myStderr);
});
