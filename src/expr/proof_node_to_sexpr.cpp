/*********************                                                        */
/*! \file proof_node_to_sexpr.cpp
 ** \verbatim
 ** Top contributors (to current version):
 **   Andrew Reynolds
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2019 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief Implementation of proof node to s-expression
 **/

#include "expr/proof_node_to_sexpr.h"

#include <iostream>

using namespace CVC4::kind;

namespace CVC4 {

ProofNodeToSExpr::ProofNodeToSExpr()
{
  NodeManager* nm = NodeManager::currentNM();
  std::vector<TypeNode> types;
  d_argsMarker = nm->mkBoundVar(":args", nm->mkSExprType(types));
}

Node ProofNodeToSExpr::convertToSExpr(std::shared_ptr<ProofNode> pn)
{
  NodeManager* nm = NodeManager::currentNM();
  std::map<std::shared_ptr<ProofNode>, Node>::iterator it;
  std::vector<std::shared_ptr<ProofNode>> visit;
  std::shared_ptr<ProofNode> cur;
  visit.push_back(pn);
  do
  {
    cur = visit.back();
    visit.pop_back();
    it = d_pnMap.find(cur);

    if (it == d_pnMap.end())
    {
      d_pnMap[cur] = Node::null();
      visit.push_back(cur);
      const std::vector<std::shared_ptr<ProofNode>>& pc = cur->getChildren();
      for (const std::shared_ptr<ProofNode>& cp : pc)
      {
        visit.push_back(cp);
      }
    }
    else if (it->second.isNull())
    {
      bool childChanged = false;
      std::vector<Node> children;
      // add proof rule
      children.push_back(getOrMkPfRuleVariable(cur->getRule()));
      const std::vector<std::shared_ptr<ProofNode>>& pc = cur->getChildren();
      for (const std::shared_ptr<ProofNode>& cp : pc)
      {
        it = d_pnMap.find(cp);
        Assert(it != d_pnMap.end());
        Assert(!it->second.isNull());
        children.push_back(it->second);
      }
      // add arguments
      const std::vector<Node>& args = cur->getArguments();
      if (!args.empty())
      {
        children.push_back(d_argsMarker);
        Node argsC = nm->mkNode(SEXPR, args);
        children.push_back(argsC);
      }
      d_pnMap[cur] = nm->mkNode(SEXPR, children);
    }
  } while (!visit.empty());
  Assert(d_pnMap.find(pn) != d_pnMap.end());
  Assert(!d_pnMap.find(pn)->second.isNull());
  return d_pnMap[pn];
}

Node ProofNodeToSExpr::getOrMkPfRuleVariable(PfRule r)
{
  std::map<PfRule, Node>::iterator it = d_pfrMap.find(r);
  if (it != d_pfrMap.end())
  {
    return it->second;
  }
  std::stringstream ss;
  ss << r;
  NodeManager* nm = NodeManager::currentNM();
  std::vector<TypeNode> types;
  Node var = nm->mkBoundVar(ss.str(), nm->mkSExprType(types));
  d_pfrMap[r] = var;
  return var;
}

}  // namespace CVC4
