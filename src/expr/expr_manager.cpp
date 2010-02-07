/*********************                                                        */
/** expr_manager.cpp
 ** Original author: dejan
 ** Major contributors: mdeters
 ** Minor contributors (to current version): none
 ** This file is part of the CVC4 prototype.
 ** Copyright (c) 2009, 2010  The Analysis of Computer Systems Group (ACSys)
 ** Courant Institute of Mathematical Sciences
 ** New York University
 ** See the file COPYING in the top-level source directory for licensing
 ** information.
 **
 ** [[ Add file-specific comments here ]]
 **/

/*
 * expr_manager.cpp
 *
 *  Created on: Dec 10, 2009
 *      Author: dejan
 */

#include "expr/node.h"
#include "expr/expr.h"
#include "expr/type.h"
#include "expr/node_manager.h"
#include "expr/expr_manager.h"

using namespace std;

namespace CVC4 {

ExprManager::ExprManager()
  : d_nm(new NodeManager()) {
}

ExprManager::~ExprManager() {
  delete d_nm;
}

const BooleanType* ExprManager::booleanType() {
  return BooleanType::getInstance();
}

const KindType* ExprManager::kindType() {
  return KindType::getInstance();
}

Expr ExprManager::mkExpr(Kind kind) {
  return Expr(this, new Node(d_nm->mkNode(kind)));
}

Expr ExprManager::mkExpr(Kind kind, const Expr& child1) {
  return Expr(this, new Node(d_nm->mkNode(kind, child1.getNode())));
}

Expr ExprManager::mkExpr(Kind kind, const Expr& child1, const Expr& child2) {
  return Expr(this, new Node(d_nm->mkNode(kind, child1.getNode(),
                                          child2.getNode())));
}

Expr ExprManager::mkExpr(Kind kind, const Expr& child1, const Expr& child2,
                         const Expr& child3) {
  return Expr(this, new Node(d_nm->mkNode(kind, child1.getNode(),
                                          child2.getNode(), child3.getNode())));
}

Expr ExprManager::mkExpr(Kind kind, const Expr& child1, const Expr& child2,
                         const Expr& child3, const Expr& child4) {
  return Expr(this, new Node(d_nm->mkNode(kind, child1.getNode(),
                                          child2.getNode(), child3.getNode(),
                                          child4.getNode())));
}

Expr ExprManager::mkExpr(Kind kind, const Expr& child1, const Expr& child2,
                         const Expr& child3, const Expr& child4,
                         const Expr& child5) {
  return Expr(this, new Node(d_nm->mkNode(kind, child1.getNode(),
                                          child2.getNode(), child3.getNode(),
                                          child5.getNode())));
}

Expr ExprManager::mkExpr(Kind kind, const vector<Expr>& children) {
  vector<Node> nodes;
  vector<Expr>::const_iterator it = children.begin();
  vector<Expr>::const_iterator it_end = children.end();
  while(it != it_end) {
    nodes.push_back(it->getNode());
    ++it;
  }
  return Expr(this, new Node(d_nm->mkNode(kind, nodes)));
}

/** Make a function type from domain to range. */
const FunctionType* 
ExprManager::mkFunctionType(const Type* domain, 
                            const Type* range) {
  return FunctionType::getInstance(this,domain,range);
}

/** Make a function type with input types from argTypes. */
const FunctionType* 
ExprManager::mkFunctionType(const std::vector<const Type*>& argTypes, 
                            const Type* range) {
  return FunctionType::getInstance(this,argTypes,range);
}

const Type* ExprManager::mkSort(std::string name) {
  // FIXME: Sorts should be unique per-ExprManager
  return new SortType(this,name);
}

Expr ExprManager::mkVar(const Type* type) {
  // FIXME: put the type into an attribute table
  return Expr(this, new Node(d_nm->mkVar()));
}

NodeManager* ExprManager::getNodeManager() const {
  return d_nm;
}

} // End namespace CVC4
