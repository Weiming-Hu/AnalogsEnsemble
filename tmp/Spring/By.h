/* 
 * File:   By.h
 * Author: Weiming Hu <cervone@psu.edu>
 *
 * Created on January 2, 2020, 5:01 PM
 */

#ifndef BY_H
#define BY_H

/**
 * The namespace By is created as tags for Boost::multi_index_container.  
 * It contains only several structs as tags for multi_index containers.
 */
namespace By {
    
    struct insert {
        /**
         * The tag for insertion sequence indexing
         */
    };

    struct ID {
        /** 
         * The tag for ID-based indexing
         */
    };

    struct name {
        /** 
         * The tag for name-based indexing
         */
    };

    struct value {
        /**
         * The tag for value-based indexing
         */
    };

    struct x {
        /** 
         * The tag for the order based on x
         */
    };

    struct y {
        /** 
         * The tag for the order based on y
         */
    };
}


#endif /* BY_H */

