//
// $Id$
//
/*
    Copyright (C) 2005- $Date$, Hammersmith Imanet Ltd
    This file is part of STIR.

    This file is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    See STIR/LICENSE.txt for details
*/
/*!
  \file 
  \ingroup BSpline
  \brief Implementation of the B-Splines Interpolation 

  \author Charalampos Tsoumpas
  \author Kris Thielemans

  $Date$
  $Revision$
*/


#include "stir/modulo.h"
#include "stir/Array.h"
#include <algorithm> // for std::fill
START_NAMESPACE_STIR
namespace BSpline {

#if defined(_MSC_VER) && _MSC_VER<=1300

  static inline 
  void set_to_zero(double& v)
  {
    v=0;
  }

  static inline 
  void set_to_zero(float& v)
  {
    v=0;
  }
#else

  template <class T>
  static inline 
  void set_to_zero(T& v)
  {
    v=0;
  }
#endif

  template <class T, int num_dimensions>
  static inline 
  void set_to_zero(Array<num_dimensions,T>& v)
  {
    v.fill(0);
  }

  template <class T, int num_dimensions>
  static inline 
  void set_to_zero(BasicCoordinate<num_dimensions,T>& c)
  {
    std::fill(c.begin(), c.end(), 0);
  }

  template <class T>
  static inline 
  void set_to_zero(std::vector<T>& v)
  {
    for (typename std::vector<T>::iterator iter = v.begin(); 
	 iter != v.end(); ++iter)
      set_to_zero(*iter);
  }

  namespace detail {
    static inline void 
    set_BSpline_values(double& z1, double& z2, double& lambda,
		       const BSplineType spline_type)
    {
      switch(spline_type)
	{
	case near_n:
	  z1=0.;
	  z2=0.;
	  break;
	case linear:
	  z1=0.;
	  z2=0.;
	  break;
	case quadratic:
	  z1 = sqrt(8.)-3.;
	  z2=0.;
	  break;
	case cubic:
	  z1 = sqrt(3.)-2.;
	  z2=0.;
	  break;
	case quartic:
	  z1 = sqrt(664.-sqrt(438976.))+sqrt(304.)-19.;
	  z2 = sqrt(664.-sqrt(438976.))-sqrt(304.)-19.;
	  break;
	case quintic:
	  z1 = 0.5*(sqrt(270.-sqrt(70980.))+sqrt(105.)-13.);
	  z2 = 0.5*(sqrt(270.-sqrt(70980.))-sqrt(105.)-13.);
	  break;
	case oMoms:
	  z1 = (sqrt(105.)-13.)/8.;	
	  z2 = 0.;		
	  break;
	}
      lambda = (1.-z1)*(1. - (1./z1));
      if (z2!=0.)
	lambda *= (1.-z2)*(1. - (1./z2));
    }
	
  } // namespace detail

  template <typename out_elemT, typename in_elemT>
  BSplines1DRegularGrid<out_elemT,in_elemT>::
  BSplines1DRegularGrid()
  { }

  template <typename out_elemT, typename in_elemT>
  BSplines1DRegularGrid<out_elemT,in_elemT>::
  BSplines1DRegularGrid(const std::vector<in_elemT> & input_vector)
  {	 
    set_private_values(cubic);	  	
    set_coef(input_vector.begin(), input_vector.end());
  }
  ///*
  template <typename out_elemT, typename in_elemT>
  BSplines1DRegularGrid<out_elemT,in_elemT>::
  BSplines1DRegularGrid(const std::vector<in_elemT> & input_vector, const BSplineType this_type)
  {	 
    set_private_values(this_type);
    set_coef(input_vector.begin(), input_vector.end());
  }

  template <typename out_elemT, typename in_elemT>
  BSplines1DRegularGrid<out_elemT,in_elemT>::
  ~BSplines1DRegularGrid()
  {}

  template <typename out_elemT, typename in_elemT>
  void
  BSplines1DRegularGrid<out_elemT,in_elemT>::
  set_private_values(BSplineType this_type)
  {
    this->spline_type = this_type;
    detail::set_BSpline_values(this->z1,this->z2,this->lambda,this_type);
  }

#if 0
  // needs to be in .h for VC 6.0
  template <typename out_elemT, typename in_elemT>
  void
  BSplines1DRegularGrid<out_elemT,in_elemT>::
  set_coef(RandIterIn input_begin_iterator, RandIterIn input_end_iterator)
  {	
    input_size = input_end_iterator - input_begin_iterator;
    for(int i=0; i<input_size; ++i)
      BSplines_coef_vector.push_back(-1); 
	
    BSplines_coef(BSplines_coef_vector.begin(),BSplines_coef_vector.end(), 
		  input_begin_iterator, input_end_iterator, z1, z2, lambda);		
    //assert (input_size==static_cast<int>(BSplines_coef_vector.size()-2));
  }
#endif

  template <typename out_elemT, typename in_elemT>
  out_elemT 
  BSplines1DRegularGrid<out_elemT,in_elemT>::
  compute_BSplines_value(const pos_type relative_position, const bool if_deriv) const
  {
    assert(relative_position>-input_size+2);
    assert(relative_position<2*input_size-4);
    out_elemT BSplines_value;
    set_to_zero(BSplines_value);
    const int int_pos =(int)floor(relative_position);
#if 0
    for (int k=int_pos-2; k<int_pos+3; ++k)		
      {	
	int index;
	// if outside-range: implement modulo(-k,2*input_size-2) by hand
	if (k<0) index=-k;
	else if (k>=input_size) index=2*input_size-2-k;
	else index = k;
	assert(0<=index && index<input_size);
	BSplines_value += BSplines_product(index, relative_position-k,if_deriv);
      }
#else
    const int kmin= int_pos-2;
    const int kmax= int_pos+2;
    const int kmax_in_range = std::min(kmax, input_size-1);
    int k=kmin;
    for (; k<0; ++k)		
      {		
	const int index=-k;
	assert(0<=index && index<input_size);
	BSplines_value += BSplines_product(index, relative_position-k,if_deriv);

      }
    for (; k<=kmax_in_range; ++k)		
      {		
	const int index=k;
	assert(0<=index && index<input_size);
	BSplines_value += BSplines_product(index, relative_position-k,if_deriv);
      }
    for (; k<=kmax; ++k)		
      {		
	const int index=2*input_size-2-k;
	assert(0<=index && index<input_size);
	BSplines_value += BSplines_product(index, relative_position-k,if_deriv);
      }
#endif
    return BSplines_value;
  }

  template <typename out_elemT, typename in_elemT>
  out_elemT 
  BSplines1DRegularGrid<out_elemT,in_elemT>::
  BSplines(const pos_type relative_position) const
  {
    return compute_BSplines_value(relative_position, false);
  }

  template <typename out_elemT, typename in_elemT>
  out_elemT 
  BSplines1DRegularGrid<out_elemT,in_elemT>::
  BSplines_1st_der(const pos_type relative_position) const
  {	
    return compute_BSplines_value(relative_position, true);
  }

  template <typename out_elemT, typename in_elemT>
  out_elemT
  BSplines1DRegularGrid<out_elemT,in_elemT>::
  BSplines_product(const int index, const pos_type relative_position, const bool if_deriv) const
  {
    if (if_deriv==true)		
      return BSplines_coef_vector[index]*BSplines_1st_der_weight(relative_position, spline_type);	
    else 	
      return BSplines_coef_vector[index]*BSplines_weights(relative_position,spline_type);	
  }

  template <typename out_elemT, typename in_elemT>
  const out_elemT BSplines1DRegularGrid<out_elemT,in_elemT>::
  operator() (const pos_type relative_position) const 
  {
    return BSplines1DRegularGrid<out_elemT,in_elemT>::
      BSplines(relative_position);		
  }

  //*
  template <typename out_elemT, typename in_elemT>
  const std::vector<out_elemT> BSplines1DRegularGrid<out_elemT,in_elemT>::
  BSplines_output_sequence(RandIterOut output_relative_position_begin_iterator,  //relative_position might be better float
			   RandIterOut output_relative_position_end_iterator)
  {
    std::vector<pos_type> output_vector(output_relative_position_end_iterator-
					output_relative_position_begin_iterator);	
	
    for(RandIterOut current_iterator=output_vector.begin(), 
	  current_relative_position_iterator=output_relative_position_begin_iterator; 
	current_iterator!=output_vector.end() && 
	  current_relative_position_iterator!=output_relative_position_end_iterator; 
	++current_iterator,++current_relative_position_iterator)
      *current_iterator = BSplines1DRegularGrid<out_elemT,in_elemT>:: 
	BSplines(*current_relative_position_iterator);		

    return output_vector;		
  }
  template <typename out_elemT, typename in_elemT>
  const std::vector<out_elemT> BSplines1DRegularGrid<out_elemT,in_elemT>::
  BSplines_output_sequence(std::vector<pos_type> output_relative_position)
  {
    return BSplines_output_sequence(output_relative_position.begin(),
				    output_relative_position.end());
  }

  template <typename in_elemT>
  void
  linear_extrapolation(std::vector<in_elemT> &input_vector) 
  {
    input_vector.push_back(*(input_vector.end()-1)*2 - *(input_vector.end()-2));
    input_vector.insert(input_vector.begin(), *input_vector.begin()*2 - *(input_vector.begin()+1));
  }

} // end BSpline namespace

END_NAMESPACE_STIR

