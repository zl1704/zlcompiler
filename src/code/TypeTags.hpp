/*
 * Type.hpp
 *
 *  Created on: 2017年12月8日
 *      Author: ZL
 */

#ifndef CODE_TYPETAGS_HPP_
#define CODE_TYPETAGS_HPP_
class TypeTags {
public:

    /** The tag of the basic type `byte'.
     */

    static const int BYTE = 1;

    /** The tag of the basic type `char'.
     */
    static const int CHAR = BYTE+1;

    /** The tag of the basic type `short'.
     */
    static const int SHORT = CHAR+1;

    /** The tag of the basic type `int'.
     */
    static const int INT = SHORT+1;

    /** The tag of the basic type `long'.
     */
    static const int LONG = INT+1;

    /** The tag of the basic type `float'.
     */
    static const int FLOAT = LONG+1;

    /** The tag of the basic type `double'.
     */
    static const int DOUBLE = FLOAT+1;

    /** The tag of the basic type `boolean'.
     */
    static const int BOOLEAN = DOUBLE+1;

    /** The tag of the type `void'.
     */
    static const int VOID = BOOLEAN+1;

    /** The tag of all class and interface types.
     */
    static const int CLASS = VOID+1;

    /** The tag of all array types.
     */
    static const int ARRAY = CLASS+1;

    /** The tag of all (monomorphic) method types.
     */
    static const int METHOD = ARRAY+1;

    /** The tag of all package "types".
     */
    static const int PACKAGE = METHOD+1;

    /** The tag of all (source-level) type variables.
     */
    static const int TYPEVAR = PACKAGE+1;

    /** The tag of all type arguments.
     */
    static const int WILDCARD = TYPEVAR+1;

    /** The tag of all polymorphic (method-) types.
     */
    static const int FORALL = WILDCARD+1;

    /** The tag of the bottom type <null>.
     */
    static const int BOT = FORALL+1;

    /** The tag of a missing type.
     */
    static const int NONE = BOT+1;

    /** The tag of the error type.
     */
    static const int ERROR = NONE+1;

    /**
     * string
     */
    static const int STRING = ERROR+1;


    /** The tag of an unknown type
     */
    static const int UNKNOWN = STRING+1;

    /** The tag of all instantiatable type variables.
     */
    static const int UNDETVAR = UNKNOWN+1;

    /** The number of type tags.
     */
    static const int TypeTagCount = UNDETVAR+1;

    /** The maximum tag of a basic type.
     */
    static const int lastBaseTag = BOOLEAN;

    /** The minimum tag of a partial type
     */
    static const int firstPartialTag = ERROR;

    static const int ROOT = 999;

    int tag;

	TypeTags();
	virtual ~TypeTags();



};


#endif /* CODE_TYPETAGS_HPP_ */
