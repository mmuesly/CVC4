/*********************                                                        */
/*! \file sygus_invariance.cpp
 ** \verbatim
 ** Top contributors (to current version):
 **   Andrew Reynolds
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2018 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief Implementation of techniques for sygus invariance tests.
 **/

#include "theory/quantifiers/sygus/sygus_invariance.h"

#include "theory/quantifiers/sygus/ce_guided_conjecture.h"
#include "theory/quantifiers/sygus/sygus_pbe.h"
#include "theory/quantifiers/sygus/term_database_sygus.h"

using namespace CVC4::kind;
using namespace std;

namespace CVC4 {
namespace theory {
namespace quantifiers {

void EvalSygusInvarianceTest::init(Node conj, Node var, Node res)
{
  d_conj = conj;
  d_var = var;
  d_result = res;
}

Node EvalSygusInvarianceTest::doEvaluateWithUnfolding(TermDbSygus* tds, Node n)
{
  return tds->evaluateWithUnfolding(n, d_visited);
}

bool EvalSygusInvarianceTest::invariant(TermDbSygus* tds, Node nvn, Node x)
{
  TNode tnvn = nvn;
  Node conj_subs = d_conj.substitute(d_var, tnvn);
  Node conj_subs_unfold = doEvaluateWithUnfolding(tds, conj_subs);
  Trace("sygus-cref-eval2-debug")
      << "  ...check unfolding : " << conj_subs_unfold << std::endl;
  Trace("sygus-cref-eval2-debug") << "  ......from : " << conj_subs
                                  << std::endl;
  if (conj_subs_unfold == d_result)
  {
    Trace("sygus-cref-eval2") << "Evaluation min explain : " << conj_subs
                              << " still evaluates to " << d_result
                              << " regardless of ";
    Trace("sygus-cref-eval2") << x << std::endl;
    return true;
  }
  return false;
}

void EquivSygusInvarianceTest::init(
    TermDbSygus* tds, TypeNode tn, CegConjecture* aconj, Node e, Node bvr)
{
  // compute the current examples
  d_bvr = bvr;
  if (aconj->getPbe()->hasExamples(e))
  {
    d_conj = aconj;
    d_enum = e;
    unsigned nex = aconj->getPbe()->getNumExamples(e);
    for (unsigned i = 0; i < nex; i++)
    {
      d_exo.push_back(d_conj->getPbe()->evaluateBuiltin(tn, bvr, e, i));
    }
  }
}

bool EquivSygusInvarianceTest::invariant(TermDbSygus* tds, Node nvn, Node x)
{
  TypeNode tn = nvn.getType();
  Node nbv = tds->sygusToBuiltin(nvn, tn);
  Node nbvr = tds->getExtRewriter()->extendedRewrite(nbv);
  Trace("sygus-sb-mexp-debug") << "  min-exp check : " << nbv << " -> " << nbvr
                               << std::endl;
  bool exc_arg = false;
  // equivalent / singular up to normalization
  if (nbvr == d_bvr)
  {
    // gives the same result : then the explanation for the child is irrelevant
    exc_arg = true;
    Trace("sygus-sb-mexp") << "sb-min-exp : " << tds->sygusToBuiltin(nvn)
                           << " is rewritten to " << nbvr;
    Trace("sygus-sb-mexp") << " regardless of the content of "
                           << tds->sygusToBuiltin(x) << std::endl;
  }
  else
  {
    if (nbvr.isVar())
    {
      TypeNode xtn = x.getType();
      if (xtn == tn)
      {
        Node bx = tds->sygusToBuiltin(x, xtn);
        Assert(bx.getType() == nbvr.getType());
        if (nbvr == bx)
        {
          Trace("sygus-sb-mexp") << "sb-min-exp : " << tds->sygusToBuiltin(nvn)
                                 << " always rewrites to argument " << nbvr
                                 << std::endl;
          // rewrites to the variable : then the explanation of this is
          // irrelevant as well
          exc_arg = true;
          d_bvr = nbvr;
        }
      }
    }
  }
  // equivalent under examples
  if (!exc_arg)
  {
    if (!d_enum.isNull())
    {
      bool ex_equiv = true;
      for (unsigned j = 0; j < d_exo.size(); j++)
      {
        Node nbvr_ex = d_conj->getPbe()->evaluateBuiltin(tn, nbvr, d_enum, j);
        if (nbvr_ex != d_exo[j])
        {
          ex_equiv = false;
          break;
        }
      }
      if (ex_equiv)
      {
        Trace("sygus-sb-mexp") << "sb-min-exp : " << tds->sygusToBuiltin(nvn);
        Trace("sygus-sb-mexp")
            << " is the same w.r.t. examples regardless of the content of "
            << tds->sygusToBuiltin(x) << std::endl;
        exc_arg = true;
      }
    }
  }
  return exc_arg;
}

bool DivByZeroSygusInvarianceTest::invariant(TermDbSygus* tds, Node nvn, Node x)
{
  TypeNode tn = nvn.getType();
  Node nbv = tds->sygusToBuiltin(nvn, tn);
  Node nbvr = tds->getExtRewriter()->extendedRewrite(nbv);
  if (tds->involvesDivByZero(nbvr))
  {
    Trace("sygus-sb-mexp") << "sb-min-exp : " << tds->sygusToBuiltin(nvn)
                           << " involves div-by-zero regardless of "
                           << tds->sygusToBuiltin(x) << std::endl;
    return true;
  }
  return false;
}

void NegContainsSygusInvarianceTest::init(Node e,
                                          std::vector<std::vector<Node> >& ex,
                                          std::vector<Node>& exo,
                                          std::vector<unsigned>& ncind)
{
  Assert(ex.size() == exo.size());
  d_enum = e;
  d_ex.insert(d_ex.end(), ex.begin(), ex.end());
  d_exo.insert(d_exo.end(), exo.begin(), exo.end());
  d_neg_con_indices.insert(d_neg_con_indices.end(), ncind.begin(), ncind.end());
}

bool NegContainsSygusInvarianceTest::invariant(TermDbSygus* tds,
                                               Node nvn,
                                               Node x)
{
  if (!d_enum.isNull())
  {
    TypeNode tn = nvn.getType();
    Node nbv = tds->sygusToBuiltin(nvn, tn);
    Node nbvr = tds->getExtRewriter()->extendedRewrite(nbv);
    // if for any of the examples, it is not contained, then we can exclude
    for (unsigned i = 0; i < d_neg_con_indices.size(); i++)
    {
      unsigned ii = d_neg_con_indices[i];
      Assert(ii < d_exo.size());
      Node nbvre = tds->evaluateBuiltin(tn, nbvr, d_ex[ii]);
      Node out = d_exo[ii];
      Node cont =
          NodeManager::currentNM()->mkNode(kind::STRING_STRCTN, out, nbvre);
      Trace("sygus-pbe-cterm-debug") << "Check: " << cont << std::endl;
      Node contr = Rewriter::rewrite(cont);
      if (contr == tds->d_false)
      {
        if (Trace.isOn("sygus-pbe-cterm"))
        {
          Trace("sygus-pbe-cterm")
              << "PBE-cterm : enumerator : do not consider ";
          Trace("sygus-pbe-cterm") << nbv << " for any "
                                   << tds->sygusToBuiltin(x) << " since "
                                   << std::endl;
          Trace("sygus-pbe-cterm") << "   PBE-cterm :    for input example : ";
          for (unsigned j = 0, size = d_ex[ii].size(); j < size; j++)
          {
            Trace("sygus-pbe-cterm") << d_ex[ii][j] << " ";
          }
          Trace("sygus-pbe-cterm") << std::endl;
          Trace("sygus-pbe-cterm")
              << "   PBE-cterm :     this rewrites to : " << nbvre << std::endl;
          Trace("sygus-pbe-cterm")
              << "   PBE-cterm : and is not in output : " << out << std::endl;
        }
        return true;
      }
      Trace("sygus-pbe-cterm-debug2")
          << "...check failed, rewrites to : " << contr << std::endl;
    }
  }
  return false;
}

} /* CVC4::theory::quantifiers namespace */
} /* CVC4::theory namespace */
} /* CVC4 namespace */
