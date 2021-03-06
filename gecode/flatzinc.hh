/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *     Guido Tack, 2007
 *
 *  Last modified:
 *     $Date: 2010-07-02 19:18:43 +1000 (Fri, 02 Jul 2010) $ by $Author: tack $
 *     $Revision: 11149 $
 *
 *  This file is part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __GECODE_FLATZINC_HH__
#define __GECODE_FLATZINC_HH__

#include <iostream>
#include <unordered_map>
#include <cassert>
#include <sstream>

#include "conexpr.hh"
#include "ast.hh"
#include "varspec.hh"

/**
 * \namespace FlatZinc
 * \brief Interpreter for the %FlatZinc language
 *
 * The Gecode::FlatZinc namespace contains all functionality required
 * to parse and solve constraint models written in the %FlatZinc language.
 *
 */

namespace FlatZinc {

const int THEORY = 9;

const int COMPOUND = 2;
const int SYMBOL = 1;
const int NUMBER = 0;

const int ELEMENT = 4;
const int ATOM = 5;
const int ATOMWITHBOUND = 6;

  class FlatZincModel;

  class IntVar
  {
  public:
      /// so 0 is not a legal variable
      IntVar() : id_(0) {}
      
      unsigned int getID() const { return id_; }
      const AST::SetLit* getDomain() const { /*return vs_->domain.some();*/ return &dom; }
      //Printer x;
      void printDefinition(FlatZincModel& m) const;
      
               
      
      
      static IntVar newVar(IntVarSpec* vs, unsigned int uid, const std::string& s)
      {
          IntVar ret;
          ret.id_ = uid;
          ret.name_ = s;
          
          assert(!vs->alias);
          //ret.vs_ = vs->domain.some();
          if (vs->assigned)
              ret.dom = AST::SetLit(vs->i,vs->i);
          else
          {
              assert(vs->domain());
              ret.dom = *(vs->domain.some());
          }
          return ret;
      }
      
  private:
      
      
      unsigned int id_;
      //IntVarSpec* vs_; /// hope the pointer lasts long enough
      AST::SetLit dom;
      std::string name_;
  };
  class BoolVar
  {
  public:
      unsigned int getID() const { return id_; }
      int uid() const { return int(id_); }

      void printDefinition(FlatZincModel &m) const {
          assert(!vs_->alias);
          /// {x}
          std::cout << "[BOOL] b" << id_ << "\n";
          /*if (vs_->assigned) {
              /// :- x or :- not x
              std::cout << "1 0 0 0 1 ";
              if (vs_->i > 0) {
                  std::cout << -((int) (id_));
              } else {
                  std::cout << id_;
              }
              std::cout << "\n";
          }*/

//          std::cout << "4 " << name_.size() << " " << name_ << " 1 " << id_ << "\n";
      }

      /// so 0 is not a legal variable
      BoolVar() : id_(0), vs_(0) {}
      static BoolVar newVar(BoolVarSpec* vs,const std::string& s)
      {
          static unsigned int num = 3; /// 1 and 2 are true and false
          BoolVar ret;
          ret.id_ = num++;
          ret.vs_ = vs;
          ret.name_ = s;
          return ret;
      }  
  private:
      unsigned int id_;
      BoolVarSpec* vs_; /// hope the pointer lasts long enough
      std::string name_;
  };
  
  class SetVar {};

  class Printer;
  


  /**
   * \brief A space that can be initialized with a %FlatZinc model
   *
   */
  class FlatZincModel {
  public:
    enum Meth {
      SAT, //< Solve as satisfaction problem
      MIN, //< Solve as minimization problem
      MAX  //< Solve as maximization problem
    };
  protected:
    /// Number of integer variables
    int intVarCount;
    /// Number of Boolean variables
    int boolVarCount;
    /// Number of set variables
    int setVarCount;

    /// Index of the integer variable to optimize
    int _optVar;
  
    /// Whether to solve as satisfaction or optimization problem
    Meth _method;
    
    /// Annotations on the solve item
    AST::Array* _solveAnnotations;

  public:
    
    int getTrueUid()
    {
        return trueUid;
    }
    
    int getFalseUid()
    {
        return falseUid;
    }
    
    
    void addSymbol(const std::string& s)
    {
        if (symbols.find(s)==symbols.end())
        {
//            std::cout << THEORY << " " << SYMBOL << " " << uid_theory << " " << s.size() << " " << s << "\n";
            symbols.emplace(s,uid_theory++);      
        }
    }
    
    int addInteger(int s)
    {
        auto x = integers.find(s);
        if (x==integers.end())
        {
//            std::cout << "%%%" << s << "\n";
            integers.emplace(s,uid_theory++);
            return uid_theory-1;
        }
        return x->second;
    }
    
    unsigned int getNewTerm()
    {
        return uid_theory++;
    }
    
    unsigned int getNewElement()
    {
        return uid_theory++;
    }
    

    unsigned int getSymbolUid(const std::string& s)
    {
        assert(symbols.find(s)!=symbols.end());
        return symbols[s];
    }
    
    unsigned int getIntegerUid(const int& s)
    {
        assert(integers.find(s)!=integers.end());
        return integers[s];
    }
    
    unsigned int getIntVariableUid(unsigned int index)
    {
        assert(iv.size()>index);
        return iv[index].getID();
    }
    
    int getBoolVariableUid(unsigned int index)
    {
        assert(bv.size()>index);
        return bv[index].uid();
    }

    /// uid cointer for theory term ids;
    unsigned int uid_theory;
    /// map for operators to uid (theory terms)
    std::unordered_map<std::string,unsigned int> symbols;
    std::unordered_map<int,unsigned int> integers;
    unsigned int falseUid;
    unsigned int trueUid;
    /// The integer variables
    std::vector<IntVar> iv;
    /// Indicates whether an integer variable is introduced by mzn2fzn
    std::vector<bool> iv_introduced;
    /// Indicates whether an integer variable aliases a Boolean variable
    std::vector<int> iv_boolalias;
    /// The Boolean variables
    std::vector<BoolVar> bv;
    /// Indicates whether a Boolean variable is introduced by mzn2fzn
    std::vector<bool> bv_introduced;
    /// The set variables
    std::vector<SetVar> sv;
    /// Indicates whether a set variable is introduced by mzn2fzn
    std::vector<bool> sv_introduced;

    /// Construct empty space
    FlatZincModel(void);
  
    /// Destructor
    ~FlatZincModel(void);
  
    /// Initialize space with given number of variables
    void init(int intVars, int boolVars, int setVars);

    /// Create new integer variable from specification
    void newIntVar(IntVarSpec* vs, const std::string &s);
    /// Link integer variable \a iv to Boolean variable \a bv
    void aliasBool2Int(int iv, int bv);
    /// Return linked Boolean variable for integer variable \a iv
    int aliasBool2Int(int iv);
    /// Create new Boolean variable from specification
    void newBoolVar(BoolVarSpec* vs, const std::string &s);
    /// Create new set variable from specification
    void newSetVar(SetVarSpec* vs);
  
    /// Post a constraint specified by \a ce
    void postConstraint(const ConExpr& ce, AST::Node* annotation);
  
    /// Post the solve item
    void solve(AST::Array* annotation);
    /// Post that integer variable \a var should be minimized
    void minimize(int var, AST::Array* annotation);
    /// Post that integer variable \a var should be maximized
    void maximize(int var, AST::Array* annotation);

    /// Run the search
    void run(std::ostream& out, const Printer& p);
  
    /// Produce output on \a out using \a p
    void print(std::ostream& out, const Printer& p) const;

    /// Return whether to solve a satisfaction or optimization problem
    Meth method(void) const;

    /// Return index of variable used for optimization
    int optVar(void) const;

    /**
     * \brief Create branchers corresponding to the solve item annotations
     *
     * If \a ignoreUnknown is true, unknown solve item annotations will be
     * ignored, otherwise a warning is written to \a err.
     */
    void createBranchers(AST::Node* ann, bool ignoreUnknown,
                         std::ostream& err = std::cerr);

    /// Return the solve item annotations
    AST::Array* solveAnnotations(void) const;

  };

  /**
   * \brief Output support class for %FlatZinc interpreter
   *
   */
  class Printer {
  private:
    AST::Array* _output;
    void printElem(std::ostream& out,
                   AST::Node* ai, const FlatZincModel& m) const;
  public:
    Printer(void) : _output(NULL) {}
    void init(AST::Array* output);

    void print(std::ostream& out, const FlatZincModel& m) const;
  
    ~Printer(void);
    
  private:
    Printer(const Printer&);
    Printer& operator=(const Printer&);
  };

  /// %Exception class for %FlatZinc errors
  class Error {
  private:
    const std::string msg;
  public:
    Error(const std::string& where, const std::string& what)
    : msg(where+": "+what) {}
    const std::string& toString(void) const { return msg; }
  };

  /**
   * \brief Parse FlatZinc file \a fileName into \a fzs and return it.
   *
   * Creates a new empty FlatZincModel if \a fzs is NULL.
   */
  FlatZincModel* parse(const std::string& fileName,
                       Printer& p, std::ostream& err = std::cerr,
                       FlatZincModel* fzs=NULL);

  /**
   * \brief Parse FlatZinc from \a is into \a fzs and return it.
   *
   * Creates a new empty FlatZincModel if \a fzs is NULL.
   */
  FlatZincModel* parse(std::istream& is,
                       Printer& p, std::ostream& err = std::cerr,
                       FlatZincModel* fzs=NULL);

}

#endif

// STATISTICS: flatzinc-any
