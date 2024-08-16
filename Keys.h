#ifndef KEY_H
#define KEY_H

#include <inttypes.h>

// For key name clarification, see https://w3c.github.io/uievents-code/#keyboard-key-codes
enum class Key : uint8_t {
    // Alphanumeric
    Backquote =             0,
    Backslash =             1,
    BracketLeft =           2,
    BracketRight =          3,
    Comma =                 4,
    Digit0 =                5,
    Digit1 =                6,
    Digit2 =                7,
    Digit3 =                8,
    Digit4 =                9,
    Digit5 =                10,
    Digit6 =                11,
    Digit7 =                12,
    Digit8 =                13,
    Digit9 =                14,
    Equal =                 15,
    IntlBackslash =         16,
    IntlRo =                17,
    IntlYen =               18,
    A =                     19, // KeyA
    B =                     20, // KeyB
    C =                     21, // KeyC
    D =                     22, // KeyD
    E =                     23, // KeyE
    F =                     24, // KeyF
    G =                     25, // KeyG
    H =                     26, // KeyH
    I =                     27, // KeyI
    J =                     28, // KeyJ
    K =                     29, // KeyK
    L =                     30, // KeyL
    M =                     31, // KeyM
    N =                     32, // KeyN
    O =                     33, // KeyO
    P =                     34, // KeyP
    Q =                     35, // KeyQ
    R =                     36, // KeyR
    S =                     37, // KeyS
    T =                     38, // KeyT
    U =                     39, // KeyU
    V =                     40, // KeyV
    W =                     41, // KeyW
    X =                     42, // KeyX
    Y =                     43, // KeyY
    Z =                     44, // KeyZ
    Minus =                 45,
    Period =                46,
    Quote =                 47,
    Semicolon =             48,
    Slash =                 49,

    // Functional
    AltLeft =               50,
    AltRight =              51,
    Backspace =             52,
    CapsLock =              53,
    ContextMenu =           54,
    ControlLeft =           55,
    ControlRight =          56,
    Enter =                 57,
    MetaLeft =              58,
    MetaRight =             59,
    ShiftLeft =             60,
    ShiftRight =            61,
    Space =                 62,
    Tab =                   63,

    // Control Pad
    Delete =                64,
    End =                   65,
    Help =                  66,
    Home =                  67,
    Insert =                68,
    PageDown =              69,
    PageUp =                70,
    ArrowDown =             71,
    ArrowLeft =             72,
    ArrowRight =            73,
    ArrowUp =               74,

    // Numpad
    NumLock =               75,
    Numpad0 =               76,
    Numpad1 =               77,
    Numpad2 =               78,
    Numpad3 =               79,
    Numpad4 =               80,
    Numpad5 =               81,
    Numpad6 =               82,
    Numpad7 =               83,
    Numpad8 =               84,
    Numpad9 =               85,
    NumpadAdd =             86,
    NumpadBackspace =       87,
    NumpadClear =           88,
    NumpadClearEntry =      89,
    NumpadComma =           90,
    NumpadDecimal =         91,
    NumpadDivide =          92,
    NumpadEnter =           93,
    NumpadEqual =           94,
    NumpadHash =            95,
    NumpadMemoryAdd =       96,
    NumpadMemoryClear =     97,
    NumpadMemoryRecall =    98,
    NumpadMemoryStore =     99,
    NumpadMemorySubtract = 100,
    NumpadMultiply =       101,
    NumpadParenLeft =      102,
    NumpadParenRight =     103,
    NumpadStar =           104,
    NumpadSubtract =       105,
};

#endif