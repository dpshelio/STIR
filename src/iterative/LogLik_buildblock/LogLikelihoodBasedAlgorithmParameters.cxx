
//
//
// $Id$: $Date$
//
/*!

  \file
  \ingroup LogLikBased_buildblock
  
  \brief  implementation of the LogLikelihoodBasedAlgorithmParameters class 
    
  \author Matthew Jacobson
  \author Kris Thielemans
  \author PARAPET project
      
  \date $Date$
        
  \version $Revision$
*/

#include "LogLikBased/LogLikelihoodBasedAlgorithmParameters.h"
#include "utilities.h"
#include <iostream>
#ifdef PROJSMOOTH
#include "stream.h"
#endif

#ifndef TOMO_NO_NAMESPACES
using std::endl;
using std::ends;
#endif

START_NAMESPACE_TOMO

//
//MJ 01/02/2000 added
//For iterative MLE algorithms
//

LogLikelihoodBasedAlgorithmParameters::LogLikelihoodBasedAlgorithmParameters()
  :IterativeReconstructionParameters()
{

 
  sensitivity_image_filename = "1";
  
  additive_projection_data_filename = "0"; // all zeroes by default

#ifdef PROJSMOOTH
  forward_proj_postsmooth_tang_kernel_double.resize(0);
  forward_proj_postsmooth_tang_kernel = VectorWithOffset<float>();
  forward_proj_postsmooth_ax_kernel_double.resize(0);
  forward_proj_postsmooth_ax_kernel = VectorWithOffset<float>();
  forward_proj_postsmooth_smooth_segment_0_axially = false;

  add_key("Forward projector postsmoothing kernel",
    KeyArgument::LIST_OF_DOUBLES, &forward_proj_postsmooth_tang_kernel_double);
  add_key("Forward projector postsmoothing tangential kernel",
    KeyArgument::LIST_OF_DOUBLES, &forward_proj_postsmooth_tang_kernel_double);
  add_key("Forward projector postsmoothing axial kernel",
    KeyArgument::LIST_OF_DOUBLES, &forward_proj_postsmooth_ax_kernel_double);
  add_key("Forward projector postsmoothing smooth segment 0 axially",
    KeyArgument::INT, &forward_proj_postsmooth_smooth_segment_0_axially);
#endif

  add_key("sensitivity image",
    KeyArgument::ASCII, &sensitivity_image_filename);

  // AZ 04/10/99 added
  add_key("additive sinogram",
    KeyArgument::ASCII, &additive_projection_data_filename);

}

void LogLikelihoodBasedAlgorithmParameters::ask_parameters()
{

  IterativeReconstructionParameters::ask_parameters();

  char additive_projection_data_filename_char[max_filename_length], sensitivity_image_filename_char[max_filename_length];
 
 
  ask_filename_with_extension(sensitivity_image_filename_char,"Enter file name of sensitivity image (1 = 1's): ", "");   
 
  sensitivity_image_filename=sensitivity_image_filename_char;

  // AZ&MJ 04/10/99 added

  ask_filename_with_extension(additive_projection_data_filename_char,"Enter file name of additive sinogram data (0 = 0's): ", "");    

  additive_projection_data_filename=additive_projection_data_filename_char;


}

bool LogLikelihoodBasedAlgorithmParameters::post_processing()
{
  if (IterativeReconstructionParameters::post_processing())
    return true;

 
  if (sensitivity_image_filename.length() == 0)
  { warning("You need to specify a sensitivity image\n"); return true; }
 
#ifdef PROJSMOOTH
  if (forward_proj_postsmooth_tang_kernel_double.size()>0)
  {
    const int max_kernel_num = forward_proj_postsmooth_tang_kernel_double.size()/2;
    const int min_kernel_num = max_kernel_num - forward_proj_postsmooth_tang_kernel_double.size()+1;
    forward_proj_postsmooth_tang_kernel.grow(min_kernel_num, max_kernel_num);

    int i=min_kernel_num;
    int j=0;
    while(i<=max_kernel_num)
         {
           forward_proj_postsmooth_tang_kernel[i++] =
             static_cast<float>(forward_proj_postsmooth_tang_kernel_double[j++]);
         }
  }
  if (forward_proj_postsmooth_ax_kernel_double.size()>0)
  {
    const int max_kernel_num = forward_proj_postsmooth_ax_kernel_double.size()/2;
    const int min_kernel_num = max_kernel_num - forward_proj_postsmooth_ax_kernel_double.size()+1;
    forward_proj_postsmooth_ax_kernel.grow(min_kernel_num, max_kernel_num);

    int i=min_kernel_num;
    int j=0;
    while(i<=max_kernel_num)
         {
           forward_proj_postsmooth_ax_kernel[i++] =
             static_cast<float>(forward_proj_postsmooth_ax_kernel_double[j++]);
         }
  }
#endif

  return false;
}

// KT&MJ 230899 changed return-type to string
string LogLikelihoodBasedAlgorithmParameters::parameter_info() const
{
  
  // TODO dangerous for out-of-range, but 'old-style' ostrstream seems to need this
  char str[10000];
  ostrstream s(str, 10000);

  s << IterativeReconstructionParameters::parameter_info();

#ifdef PROJSMOOTH
  s << "Forward projector postsmoothing tangential kernel :="
    << forward_proj_postsmooth_tang_kernel  << endl;
  s << "Forward projector postsmoothing axial kernel := "
    << forward_proj_postsmooth_ax_kernel  << endl;
  s << "Forward projector postsmoothing smooth segment 0 axially := "
    << forward_proj_postsmooth_smooth_segment_0_axially << endl;
#endif

  s << "sensitivity image := "
    << sensitivity_image_filename << endl;

 // AZ 04/10/99 added

  s << "additive sinogram := "
    << additive_projection_data_filename << endl<<endl;

  s<<ends;
  
  return s.str();
}

END_NAMESPACE_TOMO
