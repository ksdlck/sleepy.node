/* 
 * It's a pretty good idea to do as much cool stuff as you can in Javascript
 * and keep the native binding to as thin a wrapper as possible.  This is
 * beneficial particularly because V8 is not the fastest engine at making C++
 * calls from Javascript, and because Javascript will do a whole lot better at
 * playing with objects and arrays easily than C++ will, and generally can be a
 * great ally in making your life easier with what kind of validation,
 * mangling, etc., you have to do in native code.
 *
 * Oh, and if you want to export a single function as your module.exports, you
 * need to do it here, as seen below, because V8 doesn't provide a way to do
 * this in C++.
 *
 */
module.exports = require("../build/Release/sleepy").Sleepy;
