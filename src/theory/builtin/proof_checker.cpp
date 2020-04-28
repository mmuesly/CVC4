/*********************                                                        */
/*! \file proof_checker.cpp
 ** \verbatim
 ** Top contributors (to current version):
 **   Andrew Reynolds
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2019 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief Implementation of equality proof checker
 **/

#include "theory/builtin/proof_checker.h"

#include "theory/rewriter.h"

using namespace CVC4::kind;

namespace CVC4 {
namespace theory {
namespace builtin {

Node BuiltinProofRuleChecker::applyRewrite(Node n)
{
  return Rewriter::rewrite(n);
}

Node BuiltinProofRuleChecker::applySubstitution(Node n,
                                                const std::vector<Node>& exp)
{
  Node curr = n;
  // apply substitution one at a time
  for (const Node& eqp : exp)
  {
    if (eqp.isNull() || eqp.getKind() != EQUAL)
    {
      return Node::null();
    }
    TNode var = eqp[0];
    TNode subs = eqp[1];
    curr = curr.substitute(var, subs);
  }
  return curr;
}

Node BuiltinProofRuleChecker::mkAnd(const std::vector<Node>& a)
{
  if (a.empty())
  {
    return NodeManager::currentNM()->mkConst(true);
  }
  else if (a.size() == 1)
  {
    return a[0];
  }
  return NodeManager::currentNM()->mkNode(AND, a);
}

Node BuiltinProofRuleChecker::check(PfRule id,
                                    const std::vector<Node>& children,
                                    const std::vector<Node>& args)
{
  // compute what was proven
  if (id == PfRule::ASSUME)
  {
    Assert(children.empty());
    Assert(args.size() == 1);
    return args[0];
  }
  else if (id == PfRule::SCOPE)
  {
    Assert(children.size() == 1);
    if (args.empty())
    {
      // no antecedant
      return children[0];
    }
    Node ant = mkAnd(args);
    // if the conclusion is false, its the negated antencedant only
    if (children[0].isConst() && !children[0].getConst<bool>())
    {
      return NodeManager::currentNM()->mkNode(NOT, ant);
    }
    return NodeManager::currentNM()->mkNode(IMPLIES, ant, children[0]);
  }
  else if (id == PfRule::SUBS)
  {
    Assert(children.size() > 0);
    Assert(args.size() == 1);
    std::vector<Node> exp;
    for (size_t i = 0, nchild = children.size(); i < nchild; i++)
    {
      exp.push_back(children[i]);
    }
    Node res = applySubstitution(args[0], exp);
    return args[0].eqNode(res);
  }
  else if (id == PfRule::REWRITE)
  {
    Assert(children.empty());
    Assert(args.size() == 1);
    Node res = applyRewrite(args[0]);
    return args[0].eqNode(res);
  }
  else if (id == PfRule::SUBS_REWRITE)
  {
    Assert(children.size() > 0);
    Assert(args.size() == 1);
    std::vector<Node> exp;
    for (size_t i = 0, nchild = children.size(); i < nchild; i++)
    {
      exp.push_back(children[i]);
    }
    Node res = applySubstitution(args[0], exp);
    res = applyRewrite(res);
    return args[0].eqNode(res);
  }
  // no rule
  return Node::null();
}

}  // namespace builtin
}  // namespace theory
}  // namespace CVC4
