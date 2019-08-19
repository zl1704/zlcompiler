/*
 * Kinds.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef CODE_KINDS_HPP_
#define CODE_KINDS_HPP_
class Kinds{
public:
    static const int NIL = 0;

    /** The kind of package symbols.
     */
    static const int PCK = 1 << 0;

    /** The kind of type symbols (classes, interfaces and type variables).
     */
    static const int TYP = 1 << 1;

    /** The kind of variable symbols.
     */
    static const int VAR = 1 << 2;

    /** The kind of values (variables or non-variable expressions), includes VAR.
     */
    static const int VAL = (1 << 3) | VAR;

    /** The kind of methods.
     */
    static const int MTH = 1 << 4;

    /** The error kind, which includes all other kinds.
     */
    static const int ERR = (1 << 5) - 1;

    /** The set of all kinds.
     */
    static const int AllKinds = ERR;

};




#endif /* CODE_KINDS_HPP_ */
