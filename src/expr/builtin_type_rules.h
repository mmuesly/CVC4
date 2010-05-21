/*********************                                                        */
/** builtin_type_rules.cpp
 ** Original author: dejan
 ** Major contributors: none
 ** This file is part of the CVC4 prototype.
 ** Copyright (c) 2009, 2010  The Analysis of Computer Systems Group (ACSys)
 ** Courant Institute of Mathematical Sciences
 ** New York University
 ** See the file COPYING in the top-level source directory for licensing
 ** information.
 **/

#include "cvc4_private.h"

#ifndef __CVC4__BUILTIN_TYPE_RULES_H_
#define __CVC4__BUILTIN_TYPE_RULES_H_

#include "expr/node.h"
#include "expr/type_node.h"
#include "expr/expr.h"

namespace CVC4 {

class EqualityTypeRule {
  public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n) throw (TypeCheckingExceptionPrivate) {
    if (n[0].getType() != n[1].getType()) {
      throw TypeCheckingExceptionPrivate(n, "Left and right hand side of the equation are not of the same type");
    }
    return nodeManager->booleanType();
  }
};

class DistinctTypeRule {
public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n) {
    TNode::iterator child_it = n.begin();
    TNode::iterator child_it_end = n.end();
    TypeNode firstType = (*child_it).getType();
    for (++child_it; child_it != child_it_end; ++child_it) {
      if ((*child_it).getType() != firstType) {
        throw TypeCheckingExceptionPrivate(n, "Not all arguments are of the same type");
      }
    }
    return nodeManager->booleanType();
  }
};

}

#endif /* __CVC4__BUILTIN_TYPE_RULES_H_ */
