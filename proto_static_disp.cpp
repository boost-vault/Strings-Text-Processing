//Purpose:
//  Prototype a way to eliminate all dynamic dispatching in grammar.
//  IOW, instead of having rule<...> have some way to create a type
//  the represents a specific grammar.
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/map.hpp>
#include <iostream>

  enum
vocab_id
{ terminal
, nonterminal
};

  template
  < vocab_id VocabId
  >
  struct
vocabulary
;

  template
  <
  >
  struct
vocabulary
  < terminal
  >
{

      enum
    word_ids
    { ident     //identifier
    , number    //integer number literal
    , op_add    //add operator 
    , op_mult   //multiplication operator
    , par_left  //left parenthesis
    , par_right //right parenthesis
    , end_words //end-of-words sentinel
    };
      template
      < word_ids Word
      >
      struct
    variable
    {
        variable(void)
        {}
        variable(variable const&)
        {}
          template
          < class Rhs //def of this variable
          >
          boost::mpl::pair<variable,Rhs>
        operator=(Rhs const&)
        {
            typedef boost::mpl::pair<variable,Rhs> result_type;
            result_type a_result;
            return a_result;
        }
    };
    
};

  template
  < typename VocabularyVariant
  >
  struct
word_iterator
/**@brief
 *  Specializations should be iterators over elements from VocabularyVariant.
 */
;

#include <boost/mpl/vector.hpp>

    typedef 
  boost::mpl::vector
  < vocabulary<terminal>::variable<vocabulary<terminal>::ident>
  , vocabulary<terminal>::variable<vocabulary<terminal>::number>
  , vocabulary<terminal>::variable<vocabulary<terminal>::op_add> 
  , vocabulary<terminal>::variable<vocabulary<terminal>::op_mult> 
  , vocabulary<terminal>::variable<vocabulary<terminal>::par_left> 
  , vocabulary<terminal>::variable<vocabulary<terminal>::par_right> 
  , vocabulary<terminal>::variable<vocabulary<terminal>::end_words> 
  >::type
vec_term_type
;

#include <boost/variant.hpp>

    typedef
  boost::variant
  < vocabulary<terminal>::variable<vocabulary<terminal>::ident>
  , vocabulary<terminal>::variable<vocabulary<terminal>::number>
  , vocabulary<terminal>::variable<vocabulary<terminal>::op_add> 
  , vocabulary<terminal>::variable<vocabulary<terminal>::op_mult> 
  , vocabulary<terminal>::variable<vocabulary<terminal>::par_left> 
  , vocabulary<terminal>::variable<vocabulary<terminal>::par_right> 
  , vocabulary<terminal>::variable<vocabulary<terminal>::end_words> 
  >
terminal_sum_type
;

#include <vector>
#include "boost/range/iterator_range.hpp"

  template
  <
  >
  struct
word_iterator
  < terminal_sum_type
  >
  : public boost::iterator_range<std::vector<terminal_sum_type>::const_iterator>
{
        typedef
      std::vector<terminal_sum_type>
    container_type
    ;
        typedef
      container_type::const_iterator
    iter_type
    ;
        typedef
      boost::iterator_range<iter_type>
    super_type
    ;
    word_iterator(void)
    {} 
    word_iterator(container_type const& a_container)
      : super_type(a_container.begin(), a_container.end())
    {}
      void
    operator++(void)
    {
        this->advance_begin(1);
    }
      terminal_sum_type
    operator*(void)const
    {
        return this->front();
    }
};

  template
  <
  >
  struct
vocabulary
  < nonterminal
  >
{

      enum
    word_ids
    { expression
    , term
    , factor
    };
      enum
    { nout=expression+1
    };
      template
      < word_ids Word
      >
      struct
    variable
    {
        variable(void)
        {}
        variable(variable const&)
        {}
          template
          < class Rhs //def of this variable
          >
          boost::mpl::pair<variable,Rhs>
        operator=(Rhs const&)
        {
            typedef boost::mpl::pair<variable,Rhs> result_type;
            result_type a_result;
            return a_result;
        }
    };
    
};

  enum
gram_ops_0
/**@brief 'tag' for nullary grammar expressions
 */
{ gram_op_one    //identity for sequence operator, i.e. expsilon
};

  template
  < gram_ops_0 GramOp
  >
  struct
gram_expr_0
/**@brief Grammar Expression.  IOW
 *  what can occur in any part of the rhs of a production.
 */
{};

  enum
gram_ops_1
/**@brief 'tag' for unary grammar expressions
 */
{ gram_op_repeat //repeat operator, e.g.  *x in spirit
};

  template
  < gram_ops_1 GramOp
  , class Body
  >
  struct
gram_expr_1
/**@brief Grammar Expression.  IOW
 *  what can occur in any part of the rhs of a production
 *  *and* which is composed of 1 immediate subexpression.
 */
{};

  enum
gram_ops_2
/**@brief 'tag' for binary grammar expressions
 */
{ gram_op_seq    //sequence operator, e.g.   x >> y in spirit
, gram_op_alt    //alternative operator, e.g.  x | y in spirit
};

  template
  < gram_ops_2 GramOp
  , class Left
  , class Right
  >
  struct
gram_expr_2
/**@brief Grammar Expression.  IOW
 *  what can occur in any part of the rhs of a production
 *  *and* which is composed of 2 immediate subexpressions.
 */
{
    gram_expr_2(void)
    {}
};

//Define operators corresponding to gram_expr_* templates:

  template
  < class Body
  >
  gram_expr_1
  < gram_op_repeat
  , Body
  >
operator*
  ( Body const&
  )
{
    typedef gram_expr_1<gram_op_repeat,Body> result_type;
    result_type a_result;
    return a_result;
}  

  template
  < class Left
  , class Right
  >
  gram_expr_2
  < gram_op_seq
  , Left
  , Right
  >
operator&
  ( Left const&
  , Right const&
  )
{
    typedef gram_expr_2<gram_op_seq,Left,Right> result_type;
    result_type a_result;
    return a_result;
}  

  template
  < class Left
  , class Right
  >
  gram_expr_2
  < gram_op_alt
  , Left
  , Right
  >
operator|
  ( Left const&
  , Right const&
  )
{
    typedef gram_expr_2<gram_op_alt,Left,Right> result_type;
    result_type a_result;
    return a_result;
}

using namespace boost::mpl;

    typedef
  map
  < typeof
    ( vocabulary<nonterminal>::variable<vocabulary<nonterminal>::factor>()
    =   vocabulary<terminal>::variable<vocabulary<terminal>::ident>()
      | vocabulary<terminal>::variable<vocabulary<terminal>::number>()
      |   vocabulary<terminal>::variable<vocabulary<terminal>::par_left>()
        & vocabulary<nonterminal>::variable<vocabulary<nonterminal>::expression>()
        & vocabulary<terminal>::variable<vocabulary<terminal>::par_right>()
    )//end production factor = ident | number | '(' expression ')'
  , typeof
    ( vocabulary<nonterminal>::variable<vocabulary<nonterminal>::term>()
    =   vocabulary<nonterminal>::variable<vocabulary<nonterminal>::factor>()
      & *
        ( vocabulary<terminal>::variable<vocabulary<terminal>::op_mult>()
        & vocabulary<nonterminal>::variable<vocabulary<nonterminal>::factor>()
        )
    )//end: production term = factor ( '*' factor )^*
  , typeof
    ( vocabulary<nonterminal>::variable<vocabulary<nonterminal>::expression>()
    =   vocabulary<nonterminal>::variable<vocabulary<nonterminal>::term>()
      & *
        ( vocabulary<terminal>::variable<vocabulary<terminal>::op_add>()
        & vocabulary<nonterminal>::variable<vocabulary<nonterminal>::term>()
        )
    )//end production expression = term ( '+' term )^*
  >
arith_expr_gram_type
/**@brief 
 *  Grammar for arithmetic expressions
 */
;

  struct
parser_lhs
/**@brief
 *  Top class in inheritance heirarchy of parser_production's.
 */
{
        typedef
      word_iterator<terminal_sum_type>
    word_iterator_type
    ;
};

  template
  < typename Rhs
  , typename Grammar
  >
  struct
parser_rhs
;

  template
  < vocabulary<terminal>::word_ids WordId
  , typename Grammar
  >
  struct
parser_rhs
  < vocabulary<terminal>::variable<WordId>
  , Grammar
  >
{
        typedef
      typename parser_lhs::word_iterator_type
    iterator_type
    ;
      bool
    parse_rhs(iterator_type& a_iter)
    {
        if(a_iter.empty()) return false;
        terminal_sum_type next_term=*a_iter;
        if(next_term.which() != WordId) return false;
        ++a_iter;
        return true;
    }
};

  template
  < class Body
  , typename Grammar
  >
  struct
parser_rhs
  < gram_expr_1<gram_op_repeat,Body>
  , Grammar
  >
{
        typedef
      typename parser_lhs::word_iterator_type
    iterator_type
    ;
      bool
    parse_rhs(iterator_type& a_iter)
    {
        bool match=true;
        unsigned pre_tail=a_iter.size();
        while(match && my_body.parse_rhs(a_iter))
        {
            unsigned post_tail=a_iter.size();
            match=post_tail<pre_tail;
            pre_tail=post_tail;
        }
        return match;
    }
      parser_rhs<Body,Grammar>
    my_body
    ;
};

  template
  < class Left
  , class Right
  , typename Grammar
  >
  struct
parser_rhs
  < gram_expr_2<gram_op_seq,Left,Right>
  , Grammar
  >
{
        typedef
      typename parser_lhs::word_iterator_type
    iterator_type
    ;
      bool
    parse_rhs(iterator_type& a_iter)
    {
        bool match=my_left.parse_rhs(a_iter);
        if(match)  
        {
            match=my_right.parse_rhs(a_iter);
        }
        return  match;
    }
      parser_rhs<Left,Grammar>
    my_left
    ;
      parser_rhs<Right,Grammar>
    my_right
    ;
};

  template
  < class Left
  , class Right
  , typename Grammar
  >
  struct
parser_rhs
  < gram_expr_2<gram_op_alt,Left,Right>
  , Grammar
  >
{
        typedef
      typename parser_lhs::word_iterator_type
    iterator_type
    ;
      bool
    parse_rhs(iterator_type& a_iter)
    {
        bool match=my_left.parse_rhs(a_iter);
        if(!match)
        {
            match=my_right.parse_rhs(a_iter);
        }
        return  match;
    }
    
      parser_rhs<Left,Grammar>
    my_left
    ;
      parser_rhs<Right,Grammar>
    my_right
    ;
};

  template
  < vocabulary<nonterminal>::word_ids WordId
  , typename Grammar
  >
  struct
parser_rhs
  < vocabulary<nonterminal>::variable<WordId>
  , Grammar
  >
{
        typedef
      typename parser_lhs::word_iterator_type
    iterator_type
    ;
      bool
    parse_rhs(iterator_type& a_iter)
    {
        //retrieve the rhs_parser_type for WordId from Grammar:
        typedef vocabulary<nonterminal>::variable<WordId> var_type;
        typedef typename Grammar::template production_rhs<var_type>::type rhs_parser_type;
        //create instance of parser for that nonterminal:
        rhs_parser_type rhs_parser;
        //do parse & return result:
        return rhs_parser.parse_rhs(a_iter);
    }
};

#include <boost/mpl/at.hpp>

  template
  < class GramMap
  >
  struct
grammar
{
        typedef
      grammar<GramMap>
    my_type
    ;
        typedef
      parser_lhs::word_iterator_type
    word_iterator_type
    ;
      template
      < class NonTermVar
      >
      struct
    production_rhs
    /**@brief
     *  returns parser_rhs for rhs of NonTermVar production.
     */
    {
     private:
            typedef 
          typename at<GramMap,NonTermVar>::type 
        rhs_type
        ;
     public:
            typedef
          parser_rhs<rhs_type,my_type>
        type
        ;

    };
    grammar(void)
    {
        std::cout<<"grammar::CTOR\n";
    }
};

  int
main(void)
{
      enum
    terms
    { ident    =vocabulary<terminal>::ident
    , number   =vocabulary<terminal>::number
    , op_add   =vocabulary<terminal>::op_add
    , op_mult  =vocabulary<terminal>::op_mult
    , par_left =vocabulary<terminal>::par_left
    , par_right=vocabulary<terminal>::par_right
    };
    typedef grammar<arith_expr_gram_type> gram_type;
    typedef parser_lhs::word_iterator_type witer_type;
    {   std::cout<<"test factor with input=ident\n";
        witer_type::container_type wordc;
        at_c<vec_term_type,ident>::type a_ident;
        terminal_sum_type a_sum(a_ident);
        wordc.push_back(a_sum);
        witer_type wordi(wordc);
        std::cout<<"words="<<wordi.size()<<"\n";
        typedef vocabulary<nonterminal>::variable<vocabulary<nonterminal>::factor> var;
        gram_type::production_rhs<var>::type var_rhs;
        bool result=var_rhs.parse_rhs(wordi);
        std::cout<<"words="<<wordi.size()<<"\n";
        std::cout<<"result="<<result<<"\n";
    }
    {   std::cout<<"test term with input=ident * ident\n";
        witer_type::container_type wordc;
        at_c<vec_term_type,ident>::type a_ident;
        terminal_sum_type a_sum(a_ident);
        wordc.push_back(a_sum);
        wordc.push_back(terminal_sum_type(at_c<vec_term_type,op_mult>::type()));
        wordc.push_back(terminal_sum_type(at_c<vec_term_type,ident>::type()));
        witer_type wordi(wordc);
        std::cout<<"words="<<wordi.size()<<"\n";
        typedef vocabulary<nonterminal>::variable<vocabulary<nonterminal>::term> var;
        gram_type::production_rhs<var>::type var_rhs;
        bool result=var_rhs.parse_rhs(wordi);
        std::cout<<"words="<<wordi.size()<<"\n";
        std::cout<<"result="<<result<<"\n";
    }
    return 0;
}
