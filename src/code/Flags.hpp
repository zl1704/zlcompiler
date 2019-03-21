/*
 * Flags.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef CODE_FLAGS_HPP_
#define CODE_FLAGS_HPP_

class Flags{
public :
    /* Standard Java flags.
     */
    static const int PUBLIC       = 1<<0;
    static const int PRIVATE      = 1<<1;
    static const int PROTECTED    = 1<<2;
    static const int STATIC       = 1<<3;
    static const int FINAL        = 1<<4;
    static const int SYNCHRONIZED = 1<<5;
    static const int VOLATILE     = 1<<6;
    static const int TRANSIENT    = 1<<7;
    static const int NATIVE       = 1<<8;
    static const int INTERFACE    = 1<<9;
    static const int ABSTRACT     = 1<<10;
    static const int STRICTFP     = 1<<11;

    /* Flag that marks a symbol synthetic, added in classfile v49.0. */
    static const int SYNTHETIC    = 1<<12;

    /** Flag that marks attribute interfaces, added in classfile v49.0. */
    static const int ANNOTATION   = 1<<13;

    /** An enumeration type or an enumeration constant, added in
     *  classfile v49.0. */
    static const int ENUM         = 1<<14;

    static const int StandardFlags = 0x0fff;
    static const int ModifierFlags = StandardFlags & ~INTERFACE;

    // Because the following access flags are overloaded with other
    // bit positions, we translate them when reading and writing class
    // files into unique bits positions: ACC_SYNTHETIC <-> SYNTHETIC,
    // for example.
    static const int ACC_SUPER    = 0x0020;
    static const int ACC_BRIDGE   = 0x0040;
    static const int ACC_VARARGS  = 0x0080;

    /*****************************************
     * Internal compiler flags (no bits in the lower 16).
     *****************************************/

    /** Flag is set if symbol is deprecated.
     */
    static const int DEPRECATED   = 1<<17;

    /** Flag is set for a variable symbol if the variable's definition
     *  has an initializer part.
     */
    static const int HASINIT          = 1<<18;

    /** Flag is set for compiler-generated anonymous method symbols
     *  that `own' an initializer block.
     */
    static const int BLOCK            = 1<<20;

    /** Flag is set for compiler-generated abstract methods that implement
     *  an interface method (Miranda methods).
     */
    static const int IPROXY           = 1<<21;

    /** Flag is set for nested classes that do not access instance members
     *  or `this' of an outer class and therefore don't need to be passed
     *  a this$n reference.  This flag is currently set only for anonymous
     *  classes in superclass constructor calls and only for pre 1.4 targets.
     *  todo: use this flag for optimizing away this$n parameters in
     *  other cases.
     */
    static const int NOOUTERTHIS  = 1<<22;

    /** Flag is set for package symbols if a package has a member or
     *  directory and therefore exists.
     */
    static const int EXISTS           = 1<<23;

    /** Flag is set for compiler-generated compound classes
     *  representing multiple variable bounds
     */
    static const int COMPOUND     = 1<<24;

    /** Flag is set for class symbols if a class file was found for this class.
     */
    static const int CLASS_SEEN   = 1<<25;

    /** Flag is set for class symbols if a source file was found for this
     *  class.
     */
    static const int SOURCE_SEEN  = 1<<26;

    /* State flags (are reset during compilation).
     */

    /** Flag for class symbols is set and later re-set as a lock in
     *  Enter to detect cycles in the superclass/superinterface
     *  relations.  Similarly for constructor call cycle detection in
     *  Attr.
     */
    static const int LOCKED           = 1<<27;

    /** Flag for class symbols is set and later re-set to indicate that a class
     *  has been entered but has not yet been attributed.
     */
    static const int UNATTRIBUTED = 1<<28;

    /** Flag for synthesized default constructors of anonymous classes.
     */
    static const int ANONCONSTR   = 1<<29;

    /** Flag for class symbols to indicate it has been checked and found
     *  acyclic.
     */
    static const int ACYCLIC          = 1<<30;

    /** Flag that marks bridge methods.
     */
    static const long long BRIDGE          = 1L<<31;

    /** Flag that marks formal parameters.
     */
    static const long long PARAMETER   = 1L<<33;

    /** Flag that marks varargs methods.
     */
    static const long long VARARGS   = 1L<<34;

    /** Flag for annotation type symbols to indicate it has been
     *  checked and found acyclic.
     */
    static const long long ACYCLIC_ANN      = 1L<<35;

    /** Flag that marks a generated default constructor.
     */
    static const long long GENERATEDCONSTR   = 1L<<36;

    /** Flag that marks a hypothetical method that need not really be
     *  generated in the binary, but is present in the symbol table to
     *  simplify checking for erasure clashes.
     */
    static const long long HYPOTHETICAL   = 1L<<37;

    /**
     * Flag that marks an internal proprietary class.
     */
    static const long long PROPRIETARY = 1L<<38;

    /**
     * Flag that marks a a multi-catch parameter
     */
    static const long long UNION = 1L<<39;

    /**
     * Flag that marks a signature-polymorphic invoke method.
     * (These occur inside java.lang.invoke.MethodHandle.)
     */
    static const long long POLYMORPHIC_SIGNATURE = 1L<<40;

    /**
     * Flag that marks a special kind of bridge methods (the ones that
     * come from restricted supertype bounds)
     */
    static const long long OVERRIDE_BRIDGE = 1L<<41;

    /**
     * Flag that marks an 'effectively final' local variable
     */
    static const long long EFFECTIVELY_FINAL = 1L<<42;

    /**
     * Flag that marks non-override equivalent methods with the same signature
     */
    static const long long CLASH = 1L<<43;

    /** Modifier masks.
     */
    static const int
        AccessFlags           = PUBLIC | PROTECTED | PRIVATE,
        LocalClassFlags       = FINAL | ABSTRACT | STRICTFP | ENUM | SYNTHETIC,
        MemberClassFlags      = LocalClassFlags | INTERFACE | AccessFlags,
        ClassFlags            = LocalClassFlags | INTERFACE | PUBLIC | ANNOTATION,
        InterfaceVarFlags     = FINAL | STATIC | PUBLIC,
        VarFlags              = AccessFlags | FINAL | STATIC |
                                VOLATILE | TRANSIENT | ENUM,
        ConstructorFlags      = AccessFlags,
        InterfaceMethodFlags  = ABSTRACT | PUBLIC,
        MethodFlags           = AccessFlags | ABSTRACT | STATIC | NATIVE |
                                SYNCHRONIZED | FINAL | STRICTFP;
    static const long long
        LocalVarFlags         = FINAL | PARAMETER;

};



#endif /* CODE_FLAGS_HPP_ */
