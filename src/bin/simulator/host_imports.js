addToLibrary({
    /** @type {(state: boolean) => void } */
    stdin_set_input_state: (boolState) => {
        const state = Boolean(boolState);
        if (typeof Module.stdinSetInputState === "function") {
            Module.stdinSetInputState(state);
        } else {
            throw new Error(`unimplemented stdinSetInputState.`);
        }
    },
    /** @type {() => boolean} */
    stdin_is_character_available__async: true,
    stdin_is_character_available: () => Asyncify.handleAsync(async () => {
        if (typeof Module.stdinIsCharacterAvailable === "function") {
            return Module.stdinIsCharacterAvailable();
        } else {
            throw new Error(`unimplemented stdinIsCharacterAvailable.`);
        }
    }),
    /** @type {() => number} */
    stdin_get_next_character: () => {
        if (typeof Module.stdinGetNextCharacter === "function") {
            return Module.stdinGetNextCharacter();
        } else {
            throw new Error(`unimplemented stdinGetNextCharacter.`);
        }
    },
    my_imported_getline__async: true,
    my_imported_getline: (buffer, capacity) => Asyncify.handleAsync(async () => {
        if (typeof Module.readLine !== "function") {
            throw new Error(`unimplemented readLine.`);
        }
        const bytes = await Module.readLine();
        if (bytes === null) return -1;
        const n = Math.min(bytes.length, capacity - 1);
        HEAPU8.set(bytes.subarray(0, n), buffer);
        HEAPU8[buffer + n] = 0;
        return n;
    }),
    my_imported_exit: (code) => {
        if (typeof Module.onMainExited !== "function") {
            throw new Error(`unimplemented onMainExited.`);
        } else {
            return Module.onMainExited(code);
        }
    }
});