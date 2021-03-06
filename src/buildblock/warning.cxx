//
//
/*!
  \file 
 
  \brief defines the stir::warning() function

  \author Kris Thielemans
  \author PARAPET project



*/
/*
    Copyright (C) 2000 PARAPET partners
    Copyright (C) 2000- 2010, Hammersmith Imanet Ltd
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
#include "stir/warning.h"

#include <cstdarg>
#include <iostream>
#include <sstream>

#include "stir/TextWriter.h"

/* Warning: vsnprintf is only ISO C99. So your compiler might not have it.
   Visual Studio can be accomodated with the following work-around
*/
#ifdef BOOST_MSVC
#define vsnprintf _vsnprintf
#endif

START_NAMESPACE_STIR

void warning(const char *const s, ...)
{
  va_list ap;
  va_start(ap, s);
  const unsigned size=10000;
  char tmp[size];
  const int returned_size= vsnprintf(tmp,size, s, ap);
  std::stringstream ss;
  if (returned_size < 0)
	  ss << "\nWARNING: error formatting warning message" << std::endl;
  else
  {
	  ss << "\nWARNING: " << tmp << std::endl;
      if (static_cast<unsigned>(returned_size)>=size)
		  ss << "\nWARNING: previous warning message truncated as it exceeds "
		  << size << "bytes" << std::endl;
  }
  writeText(ss.str().c_str(), WARNING_CHANNEL);
}

END_NAMESPACE_STIR
